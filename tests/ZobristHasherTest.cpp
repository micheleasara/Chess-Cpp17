#include "pch.h"
#include "Zobrist.hpp"

using Chess::Coordinates;

class ZobristHasherTest : public ::testing::Test {
protected:
    Chess::ZobristHasher hasher = Chess::ZobristHasher(8, 8);
};

TEST_F(ZobristHasherTest, throwsIfWidthOrHeightAreNotPositive) {
  EXPECT_THROW(Chess::ZobristHasher(0, 1), std::invalid_argument);
  EXPECT_THROW(Chess::ZobristHasher(1, 0), std::invalid_argument);
  EXPECT_THROW(Chess::ZobristHasher(0, 0), std::invalid_argument);
}

TEST_F(ZobristHasherTest, hashChangesIfBoardStateChanges) {
    auto initial = hasher.hash();
    hasher.pieceHasMoved(Coordinates(0, 1), Coordinates(0, 3));
    EXPECT_NE(initial, hasher.hash());
}

TEST_F(ZobristHasherTest, hashDoesNotChangeIfBoardAndPlayerAreTheSame) {
    auto h1 = hasher.hash();
    hasher.pieceHasMoved(Coordinates(1, 0), Coordinates(2, 2));
    hasher.pieceHasMoved(Coordinates(2, 2), Coordinates(1, 0));
    EXPECT_EQ(h1, hasher.hash());
}

TEST_F(ZobristHasherTest, pawnMoveChangesBoardStateRegardlessOfPosition) {
  auto h1 = hasher.hash();
  hasher.pieceHasMoved(Coordinates(0, 1), Coordinates(0, 2));
  hasher.pieceHasMoved(Coordinates(0, 2), Coordinates(0, 1));
  EXPECT_NE(h1, hasher.hash());
  h1 = hasher.hash();
  hasher.pieceHasMoved(Coordinates(0, 6), Coordinates(0, 4));
  hasher.pieceHasMoved(Coordinates(0, 4), Coordinates(0, 6));
  EXPECT_NE(h1, hasher.hash());
}

TEST_F(ZobristHasherTest, rookMoveChangesBoardStateRegardlessOfPosition) {
  auto h1 = hasher.hash();
  hasher.pieceHasMoved(Coordinates(0, 0), Coordinates(0, 2));
  hasher.pieceHasMoved(Coordinates(0, 2), Coordinates(0, 0));
  EXPECT_NE(h1, hasher.hash());
  h1 = hasher.hash();
  hasher.pieceHasMoved(Coordinates(0, 7), Coordinates(0, 5));
  hasher.pieceHasMoved(Coordinates(0, 5), Coordinates(0, 7));
  EXPECT_NE(h1, hasher.hash());
}

TEST_F(ZobristHasherTest, kingMoveChangesBoardStateRegardlessOfPosition) {
  auto h1 = hasher.hash();
  hasher.pieceHasMoved(Coordinates(4, 0), Coordinates(0, 2));
  hasher.pieceHasMoved(Coordinates(0, 2), Coordinates(4, 0));
  EXPECT_NE(h1, hasher.hash());
  h1 = hasher.hash();
  hasher.pieceHasMoved(Coordinates(4, 7), Coordinates(4, 5));
  hasher.pieceHasMoved(Coordinates(4, 5), Coordinates(4, 7));
  EXPECT_NE(h1, hasher.hash());
}

TEST_F(ZobristHasherTest, enPassantRightIsConsideredForHash) {
  hasher.pieceHasMoved(Coordinates(0, 1), Coordinates(0, 4));
  hasher.pieceHasMoved(Coordinates(1, 6), Coordinates(1, 4));
  auto enPassantHash = hasher.hash();
  hasher.pieceHasMoved(Coordinates(6, 0), Coordinates(5, 2));
  hasher.pieceHasMoved(Coordinates(5, 2), Coordinates(6, 0));
  EXPECT_NE(enPassantHash, hasher.hash());
}

TEST_F(ZobristHasherTest, promotionChangesBoardState) {
  auto prev = hasher.hash();
  hasher.replacedWithPromotion(Coordinates(7, 7),
    Chess::PromotionOption::Queen, Chess::Piece::Colour::White);
  auto current = hasher.hash();
  EXPECT_NE(prev, current);

  prev = current;
  hasher.replacedWithPromotion(Coordinates(7, 7),
    Chess::PromotionOption::Knight, Chess::Piece::Colour::White);
  current = hasher.hash();
  EXPECT_NE(prev, current);

  prev = current;
  hasher.replacedWithPromotion(Coordinates(7, 7),
    Chess::PromotionOption::Bishop, Chess::Piece::Colour::White);
  current = hasher.hash();
  EXPECT_NE(prev, current);

  prev = current;
  hasher.replacedWithPromotion(Coordinates(7, 7),
    Chess::PromotionOption::Rook, Chess::Piece::Colour::White);
  current = hasher.hash();
  EXPECT_NE(prev, current);
}

TEST_F(ZobristHasherTest, throwsIfCoordinatesAreOutOfBounds) {
  EXPECT_THROW(hasher.pieceHasMoved(Coordinates(-1, 0), Coordinates(0, 0)),
               std::out_of_range);
  EXPECT_THROW(hasher.pieceHasMoved(Coordinates(0, 8), Coordinates(0, 0)),
               std::out_of_range);
  EXPECT_THROW(hasher.pieceHasMoved(Coordinates(0, 0), Coordinates(0, -1)),
               std::out_of_range);
  EXPECT_THROW(hasher.pieceHasMoved(Coordinates(0, 0), Coordinates(8, 0)),
               std::out_of_range);
}

TEST_F(ZobristHasherTest, hashDoesNotChangeIfAttemptToMoveEmptySquare) {
  auto originalHash = hasher.hash();
  hasher.pieceHasMoved(Coordinates(3, 3), Coordinates(0, 0));
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