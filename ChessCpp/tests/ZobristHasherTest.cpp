#include "pch.h"
#include "Zobrist.hpp"

using Chess::Coordinates;
using Chess::ZobristHasher;

class ZobristHasherTest : public ::testing::Test {
protected:
    ZobristHasher hasher = ZobristHasher();
};

TEST_F(ZobristHasherTest, hashChangesIfBoardStateChanges) {
    auto initial = hasher.hash();
    hasher.pieceMoved(Coordinates(0, 1), Coordinates(0, 3));
    EXPECT_NE(initial, hasher.hash());
}

TEST_F(ZobristHasherTest, hashDoesNotChangeIfBoardAndPlayerAreTheSame) {
    auto h1 = hasher.hash();
    hasher.pieceMoved(Coordinates(1, 0), Coordinates(2, 2));
    hasher.pieceMoved(Coordinates(2, 2), Coordinates(1, 0));
    EXPECT_EQ(h1, hasher.hash());
}

TEST_F(ZobristHasherTest, pawnMoveChangesBoardStateRegardlessOfPosition) {
  auto h1 = hasher.hash();
  hasher.pieceMoved(Coordinates(0, 1), Coordinates(0, 2));
  hasher.pieceMoved(Coordinates(0, 2), Coordinates(0, 1));
  EXPECT_NE(h1, hasher.hash());
  h1 = hasher.hash();
  hasher.pieceMoved(Coordinates(0, 6), Coordinates(0, 4));
  hasher.pieceMoved(Coordinates(0, 4), Coordinates(0, 6));
  EXPECT_NE(h1, hasher.hash());
}

TEST_F(ZobristHasherTest, rookMoveChangesBoardStateRegardlessOfPosition) {
  auto h1 = hasher.hash();
  hasher.pieceMoved(Coordinates(0, 0), Coordinates(0, 2));
  hasher.pieceMoved(Coordinates(0, 2), Coordinates(0, 0));
  EXPECT_NE(h1, hasher.hash());
  h1 = hasher.hash();
  hasher.pieceMoved(Coordinates(0, 7), Coordinates(0, 5));
  hasher.pieceMoved(Coordinates(0, 5), Coordinates(0, 7));
  EXPECT_NE(h1, hasher.hash());
}

TEST_F(ZobristHasherTest, kingMoveChangesBoardStateRegardlessOfPosition) {
  auto h1 = hasher.hash();
  hasher.pieceMoved(Coordinates(4, 0), Coordinates(0, 2));
  hasher.pieceMoved(Coordinates(0, 2), Coordinates(4, 0));
  EXPECT_NE(h1, hasher.hash());
  h1 = hasher.hash();
  hasher.pieceMoved(Coordinates(4, 7), Coordinates(4, 5));
  hasher.pieceMoved(Coordinates(4, 5), Coordinates(4, 7));
  EXPECT_NE(h1, hasher.hash());
}

TEST_F(ZobristHasherTest, enPassantRightIsConsideredForHash) {
  hasher.pieceMoved(Coordinates(0, 1), Coordinates(0, 4));
  hasher.pieceMoved(Coordinates(1, 6), Coordinates(1, 4));
  auto enPassantHash = hasher.hash();
  hasher.pieceMoved(Coordinates(6, 0), Coordinates(5, 2));
  hasher.pieceMoved(Coordinates(5, 2), Coordinates(6, 0));
  EXPECT_NE(enPassantHash, hasher.hash());
}

TEST_F(ZobristHasherTest, promotionChangesBoardState) {
  auto prev = hasher.hash();
  hasher.replacedWithPromotion(Coordinates(7, 7),
    Chess::PromotionOption::Queen, Chess::Colour::White);
  auto current = hasher.hash();
  EXPECT_NE(prev, current);

  prev = current;
  hasher.replacedWithPromotion(Coordinates(7, 7),
    Chess::PromotionOption::Knight, Chess::Colour::White);
  current = hasher.hash();
  EXPECT_NE(prev, current);

  prev = current;
  hasher.replacedWithPromotion(Coordinates(7, 7),
    Chess::PromotionOption::Bishop, Chess::Colour::White);
  current = hasher.hash();
  EXPECT_NE(prev, current);

  prev = current;
  hasher.replacedWithPromotion(Coordinates(7, 7),
    Chess::PromotionOption::Rook, Chess::Colour::White);
  current = hasher.hash();
  EXPECT_NE(prev, current);
}

TEST_F(ZobristHasherTest, throwsIfCoordinatesAreOutOfBounds) {
  EXPECT_THROW(hasher.pieceMoved(Coordinates(-1, 0), Coordinates(0, 0)),
               std::out_of_range);
  EXPECT_THROW(hasher.pieceMoved(Coordinates(0, 8), Coordinates(0, 0)),
               std::out_of_range);
  EXPECT_THROW(hasher.pieceMoved(Coordinates(0, 0), Coordinates(0, -1)),
               std::out_of_range);
  EXPECT_THROW(hasher.pieceMoved(Coordinates(0, 0), Coordinates(8, 0)),
               std::out_of_range);
}

TEST_F(ZobristHasherTest, hashDoesNotChangeIfAttemptToMoveEmptySquare) {
  auto originalHash = hasher.hash();
  hasher.pieceMoved(Coordinates(3, 3), Coordinates(0, 0));
  EXPECT_EQ(originalHash, hasher.hash());
}

TEST_F(ZobristHasherTest, hashChangesIfPlayerChanges) {
  auto originalHash = hasher.hash();
  hasher.togglePlayer();
  EXPECT_NE(originalHash, hasher.hash());
}

TEST_F(ZobristHasherTest, togglingPlayerAnEvenNumberOfTimesDoesNotChangeHash) {
  auto originalHash = hasher.hash();
  hasher.togglePlayer();
  hasher.togglePlayer();
  EXPECT_EQ(originalHash, hasher.hash());
}

TEST_F(ZobristHasherTest, undoingWithNoMovesDoesNothing) {
  auto originalHash = hasher.hash();
  EXPECT_NO_THROW(hasher.restorePreviousHash());
  EXPECT_EQ(originalHash, hasher.hash());
}

TEST_F(ZobristHasherTest, undoingNormalNonCapturingMoveRestoresState) {
  auto beforeMoveHash = hasher.hash();
  hasher.pieceMoved(Coordinates(0, 1), Coordinates(0, 2));
  auto afterMoveHash = hasher.hash();

  hasher.restorePreviousHash();
  EXPECT_EQ(beforeMoveHash, hasher.hash());

  hasher.pieceMoved(Coordinates(0, 1), Coordinates(0, 2));
  EXPECT_EQ(afterMoveHash, hasher.hash());
}

TEST_F(ZobristHasherTest, undoingNormalCapturingMoveRestoresState) {
  auto beforeMoveHash = hasher.hash();
  hasher.pieceMoved(Coordinates(0, 1), Coordinates(0, 6));
  auto afterMoveHash = hasher.hash();

  hasher.restorePreviousHash();
  EXPECT_EQ(beforeMoveHash, hasher.hash());

  hasher.pieceMoved(Coordinates(0, 1), Coordinates(0, 6));
  EXPECT_EQ(afterMoveHash, hasher.hash());
}

TEST_F(ZobristHasherTest, undoingPromotionRestoresPreviousState) {
  auto beforeMoveHash = hasher.hash();
  hasher.replacedWithPromotion(Coordinates(7, 7),
                    Chess::PromotionOption::Queen, Chess::Colour::White);
  auto afterMoveHash = hasher.hash();

  hasher.restorePreviousHash();
  EXPECT_EQ(beforeMoveHash, hasher.hash());

  hasher.replacedWithPromotion(Coordinates(7, 7),
                    Chess::PromotionOption::Queen, Chess::Colour::White);
  EXPECT_EQ(afterMoveHash, hasher.hash());
}

TEST_F(ZobristHasherTest, undoingCanRestoreEnPassantState) {
  hasher.pieceMoved(Coordinates(0, 1), Coordinates(0, 4));
  hasher.pieceMoved(Coordinates(1, 6), Coordinates(1, 4));
  auto enPassantHash = hasher.hash();
  hasher.pieceMoved(Coordinates(6, 0), Coordinates(5, 2));
  auto notEnPassantHash = hasher.hash();

  hasher.restorePreviousHash();
  EXPECT_EQ(enPassantHash, hasher.hash());

  hasher.pieceMoved(Coordinates(6, 0), Coordinates(5, 2));
  EXPECT_EQ(notEnPassantHash, hasher.hash());
}

TEST_F(ZobristHasherTest, enPassantCanBeUndone) {
  hasher.pieceMoved(Coordinates(0, 1), Coordinates(0, 4));
  hasher.pieceMoved(Coordinates(1, 6), Coordinates(1, 4));
  auto beforeEnPassantHash = hasher.hash();
  hasher.pieceMoved(Coordinates(0, 4), Coordinates(1, 5));
  auto intermediateHash = hasher.hash();
  hasher.removed(Coordinates(1, 4));
  auto enPassantHash = hasher.hash();

  hasher.restorePreviousHash();
  EXPECT_EQ(intermediateHash, hasher.hash());

  hasher.restorePreviousHash();
  EXPECT_EQ(beforeEnPassantHash, hasher.hash());

  hasher.pieceMoved(Coordinates(0, 4), Coordinates(1, 5));
  hasher.removed(Coordinates(1, 4));
  EXPECT_EQ(enPassantHash, hasher.hash());
}

TEST_F(ZobristHasherTest, removingInvalidCoordinatesThrows) {
  auto originalHash = hasher.hash();
  EXPECT_THROW(hasher.removed(Coordinates(99,99)), std::out_of_range);
  EXPECT_EQ(originalHash, hasher.hash());
}

TEST_F(ZobristHasherTest, deletingEmptySquareDoesNotChangeHash) {
  auto originalHash = hasher.hash();
  hasher.removed(Coordinates(0,2));
  EXPECT_EQ(originalHash, hasher.hash());
}

TEST_F(ZobristHasherTest, deletingNonEmptySquareChangesHash) {
  auto originalHash = hasher.hash();
  hasher.removed(Coordinates(0,1));
  EXPECT_NE(originalHash, hasher.hash());
}

TEST_F(ZobristHasherTest, canBeInstantiatedWithANonStandardInitialConfiguration) {
  auto hasher = ZobristHasher({}, {Coordinates(2, 3), Coordinates(1, 2),
    Coordinates(2, 2)}, {}, {}, {}, Coordinates(1,1), {}, {}, {}, {},
    {}, Coordinates(7,7));
  auto oldHash = hasher.hash();
  hasher.removed(Coordinates(0, 0)); // remove non-existing piece
  EXPECT_EQ(hasher.hash(), oldHash);

  oldHash = hasher.hash();
  hasher.removed(Coordinates(2, 2)); // remove existing piece
  EXPECT_NE(hasher.hash(), oldHash);
}

TEST_F(ZobristHasherTest, throwsIfNonStandardInitialisationHasRepeatedCoordinates) {
  EXPECT_THROW(auto hasher = ZobristHasher({},
    {Coordinates(2, 3), Coordinates(2, 3), Coordinates(2, 2)}, {}, {}, {},
    Coordinates(1,1), {}, {}, {}, {}, {},
    Coordinates(7,7));, std::invalid_argument);

  EXPECT_THROW(auto hasher = ZobristHasher({},
    {Coordinates(2, 3), Coordinates(1, 2), Coordinates(2, 2)}, {}, {}, {},
    Coordinates(1,1), {}, {}, {}, {}, {}, 
    Coordinates(1,2));, std::invalid_argument);
}

TEST_F(ZobristHasherTest, throwsIfNonStandardInitialisationHasInvalidCoordinates) {
  EXPECT_THROW(auto hasher = ZobristHasher({},
    {Coordinates(-2, 3), Coordinates(2, 3),
    Coordinates(2, 2)}, {}, {}, {}, Coordinates(1,1), {}, {}, {}, {},
    {}, Coordinates(7,7));, std::invalid_argument);

  EXPECT_THROW(auto hasher = ZobristHasher({},
    {Coordinates(2, -3), Coordinates(2, 3),
    Coordinates(2, 2)}, {}, {}, {}, Coordinates(1,1), {}, {}, {}, {},
    {}, Coordinates(7,7));, std::invalid_argument);

  EXPECT_THROW(auto hasher = ZobristHasher({},
    {Coordinates(2, 9), Coordinates(1, 2), Coordinates(2, 2)}, {}, {}, {},
    Coordinates(1,1), {}, {}, {}, {}, {},
    Coordinates(1,2));, std::invalid_argument);

  EXPECT_THROW(auto hasher = ZobristHasher({},
    {Coordinates(9, 3), Coordinates(1, 2), Coordinates(2, 2)}, {}, {}, {},
    Coordinates(1,1), {}, {}, {}, {}, {},
    Coordinates(1,2));, std::invalid_argument);
}

TEST_F(ZobristHasherTest, rooksInitialisedInNonStandardPositionHaveMoved) {
  auto hasher = ZobristHasher({}, {Coordinates(2, 3)},
    {}, {}, {}, Coordinates(1,1), {}, {Coordinates(6,6)}, {}, {},
    {}, Coordinates(7,7));
  auto oldHash = hasher.hash();
  hasher.pieceMoved(Coordinates(2,3), Coordinates(3,3));
  hasher.pieceMoved(Coordinates(3,3), Coordinates(2,3));
  EXPECT_EQ(hasher.hash(), oldHash);

  oldHash = hasher.hash();
  hasher.pieceMoved(Coordinates(6,6), Coordinates(6,5));
  hasher.pieceMoved(Coordinates(6,5), Coordinates(6,6));
  EXPECT_EQ(hasher.hash(), oldHash);
}

TEST_F(ZobristHasherTest, pawnsInitialisedInNonStandardPositionHaveMoved) {
  auto hasher = ZobristHasher({Coordinates(5,5)}, {},
    {}, {}, {}, Coordinates(1,1), {Coordinates(3,3)}, {}, {}, {},
    {}, Coordinates(7,7));
  auto oldHash = hasher.hash();
  hasher.pieceMoved(Coordinates(5,5), Coordinates(5,6));
  hasher.pieceMoved(Coordinates(5,6), Coordinates(5,5));
  EXPECT_EQ(hasher.hash(), oldHash);

  oldHash = hasher.hash();
  hasher.pieceMoved(Coordinates(3,3), Coordinates(3,2));
  hasher.pieceMoved(Coordinates(3,2), Coordinates(3,3));
  EXPECT_EQ(hasher.hash(), oldHash);
}

TEST_F(ZobristHasherTest, kingsInitialisedInNonStandardPositionHaveMoved) {
  auto hasher = ZobristHasher({}, {}, {}, {}, {}, Coordinates(1,1),
    {}, {}, {}, {}, {}, Coordinates(7,7));
  auto oldHash = hasher.hash();
  hasher.pieceMoved(Coordinates(1,1), Coordinates(1,2));
  hasher.pieceMoved(Coordinates(1,2), Coordinates(1,1));
  EXPECT_EQ(hasher.hash(), oldHash);

  oldHash = hasher.hash();
  hasher.pieceMoved(Coordinates(7,7), Coordinates(7,6));
  hasher.pieceMoved(Coordinates(7,6), Coordinates(7,7));
  EXPECT_EQ(hasher.hash(), oldHash);
}

TEST_F(ZobristHasherTest, hashIsTheSameAfterReset) {
  hasher.pieceMoved(Coordinates(1,2), Coordinates(2,2));
  auto originalHash = hasher.hash();
  hasher.reset();
  hasher.pieceMoved(Coordinates(1,2), Coordinates(2,2));
  EXPECT_EQ(hasher.hash(), originalHash);
}