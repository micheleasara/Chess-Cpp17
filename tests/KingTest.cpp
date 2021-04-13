#include "pch.h"
#include "Board.hpp"

using Chess::Piece;
using Chess::Coordinates;
using Chess::Board;
using Chess::MoveResult;
using Chess::InvalidMove;

static auto constexpr KING_COORD = Coordinates(3,3);

class KingTest : public ::testing::Test {
protected:
  Board board = Board({Coordinates(7,6)}, {}, {},{}, {}, KING_COORD, {}, {}, 
                       {}, {}, {}, Coordinates(5,7));
  Piece const* king = nullptr;

  void SetUp() {
    auto kingOpt = board.at(KING_COORD);
    ASSERT_TRUE(kingOpt.has_value());
    king = &(kingOpt->get());
    ASSERT_FALSE(board.isGameOver());
  }
};

TEST_F(KingTest, canMoveBackwardVerticallyByOne) {
  EXPECT_TRUE(king->isMovePlausible(KING_COORD, Coordinates(3, 2)));
  EXPECT_NO_THROW(board.move(KING_COORD, Coordinates(3, 2)));
}

TEST_F(KingTest, canMoveForwardVerticallyByOne) {
  EXPECT_TRUE(king->isMovePlausible(KING_COORD, Coordinates(3, 4)));
  EXPECT_NO_THROW(board.move(KING_COORD, Coordinates(3, 4)));
}

TEST_F(KingTest, canMoveBackwardHorizontallyByOne) {
  EXPECT_TRUE(king->isMovePlausible(KING_COORD, Coordinates(2, 3)));
  EXPECT_NO_THROW(board.move(KING_COORD, Coordinates(2, 3)));
}

TEST_F(KingTest, canMoveForwardHorizontallyByOne) {
  EXPECT_TRUE(king->isMovePlausible(KING_COORD, Coordinates(4, 3)));
  EXPECT_NO_THROW(board.move(KING_COORD, Coordinates(4, 3)));
}

TEST_F(KingTest, canMoveDiagonallyForwardByOne) {
  EXPECT_TRUE(king->isMovePlausible(KING_COORD, Coordinates(4, 4)));
  EXPECT_NO_THROW(board.move(KING_COORD, Coordinates(4, 4)));
}

TEST_F(KingTest, canMoveDiagonallyBackwardByOne) {
  EXPECT_TRUE(king->isMovePlausible(KING_COORD, Coordinates(2, 2)));
  EXPECT_NO_THROW(board.move(KING_COORD, Coordinates(2, 2)));
}

TEST_F(KingTest, cannotMoveInStraightLinesByMoreThanOne) {
  EXPECT_FALSE(king->isMovePlausible(KING_COORD, Coordinates(3, 5)));
  EXPECT_THROW(board.move(KING_COORD, Coordinates(3, 5)), InvalidMove);

  EXPECT_FALSE(king->isMovePlausible(KING_COORD, Coordinates(3, 1)));
  EXPECT_THROW(board.move(KING_COORD, Coordinates(3, 1)), InvalidMove);

  EXPECT_FALSE(king->isMovePlausible(KING_COORD, Coordinates(5, 3)));
  EXPECT_THROW(board.move(KING_COORD, Coordinates(5, 3)), InvalidMove);

  EXPECT_FALSE(king->isMovePlausible(KING_COORD, Coordinates(1, 3)));
  EXPECT_THROW(board.move(KING_COORD, Coordinates(1, 3)), InvalidMove);
}

TEST_F(KingTest, cannotMoveInDiagonalLinesByMoreThanOne) {
  EXPECT_FALSE(king->isMovePlausible(KING_COORD, Coordinates(6, 6)));
  EXPECT_THROW(board.move(KING_COORD, Coordinates(6, 6)), InvalidMove);

  EXPECT_FALSE(king->isMovePlausible(KING_COORD, Coordinates(0, 0)));
  EXPECT_THROW(board.move(KING_COORD, Coordinates(0, 0)), InvalidMove);
}

TEST_F(KingTest, cannotMoveInLShape) {
  EXPECT_FALSE(king->isMovePlausible(KING_COORD, Coordinates(4, 5)));
  EXPECT_THROW(board.move(KING_COORD, Coordinates(4, 5)), InvalidMove);

  EXPECT_FALSE(king->isMovePlausible(KING_COORD, Coordinates(2, 5)));
  EXPECT_THROW(board.move(KING_COORD, Coordinates(2, 5)), InvalidMove);

  EXPECT_FALSE(king->isMovePlausible(KING_COORD, Coordinates(4, 1)));
  EXPECT_THROW(board.move(KING_COORD, Coordinates(4, 1)), InvalidMove);
  
  EXPECT_FALSE(king->isMovePlausible(KING_COORD, Coordinates(2, 1)));
  EXPECT_THROW(board.move(KING_COORD, Coordinates(2, 1)), InvalidMove);

  EXPECT_FALSE(king->isMovePlausible(KING_COORD, Coordinates(5, 4)));
  EXPECT_THROW(board.move(KING_COORD, Coordinates(5, 4)), InvalidMove);

  EXPECT_FALSE(king->isMovePlausible(KING_COORD, Coordinates(5, 2)));
  EXPECT_THROW(board.move(KING_COORD, Coordinates(5, 2)), InvalidMove);

  EXPECT_FALSE(king->isMovePlausible(KING_COORD, Coordinates(1, 4)));
  EXPECT_THROW(board.move(KING_COORD, Coordinates(1, 4)), InvalidMove);

  EXPECT_FALSE(king->isMovePlausible(KING_COORD, Coordinates(1, 2)));
  EXPECT_THROW(board.move(KING_COORD, Coordinates(1, 2)), InvalidMove);
}