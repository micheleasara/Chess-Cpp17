#include "pch.h"
#include "Board.hpp"

using Chess::Piece;
using Chess::Coordinates;
using Chess::Board;
using Chess::MoveResult;
using Chess::InvalidMove;

static auto constexpr ROOK_COORD = Coordinates(3,3);

class RookTest : public ::testing::Test {
protected:
  Board board = Board({}, {ROOK_COORD}, {}, {}, {}, Coordinates(5,1), 
                      {}, {}, {}, {}, {}, Coordinates(5,7));
  Piece const* rook = nullptr;

  void SetUp() {
    rook = board.at(ROOK_COORD);
    ASSERT_FALSE(rook == nullptr);
    ASSERT_FALSE(board.isGameOver());
  }
};

TEST_F(RookTest, canMoveBackwardVertically) {
  EXPECT_TRUE(rook->isNormalMove(ROOK_COORD, Coordinates(3, 1)));
  EXPECT_NO_THROW(board.move(ROOK_COORD, Coordinates(3, 1)));
}

TEST_F(RookTest, canMoveForwardVertically) {
  EXPECT_TRUE(rook->isNormalMove(ROOK_COORD, Coordinates(3, 5)));
  EXPECT_NO_THROW(board.move(ROOK_COORD, Coordinates(3, 5)));
}

TEST_F(RookTest, canMoveBackwardHorizontally) {
  EXPECT_TRUE(rook->isNormalMove(ROOK_COORD, Coordinates(1, 3)));
  EXPECT_NO_THROW(board.move(ROOK_COORD, Coordinates(1, 3)));
}

TEST_F(RookTest, canMoveForwardHorizontally) {
  EXPECT_TRUE(rook->isNormalMove(ROOK_COORD, Coordinates(5, 3)));
  EXPECT_NO_THROW(board.move(ROOK_COORD, Coordinates(5, 3)));
}

TEST_F(RookTest, cannotMoveInDiagonalLines) {
  EXPECT_FALSE(rook->isNormalMove(ROOK_COORD, Coordinates(6, 6)));
  EXPECT_THROW(board.move(ROOK_COORD, Coordinates(6, 6)), InvalidMove);

  EXPECT_FALSE(rook->isNormalMove(ROOK_COORD, Coordinates(0, 0)));
  EXPECT_THROW(board.move(ROOK_COORD, Coordinates(0, 0)), InvalidMove);
}

TEST_F(RookTest, cannotMoveInLShape) {
  EXPECT_FALSE(rook->isNormalMove(ROOK_COORD, Coordinates(4, 5)));
  EXPECT_THROW(board.move(ROOK_COORD, Coordinates(4, 5)), InvalidMove);

  EXPECT_FALSE(rook->isNormalMove(ROOK_COORD, Coordinates(2, 5)));
  EXPECT_THROW(board.move(ROOK_COORD, Coordinates(2, 5)), InvalidMove);

  EXPECT_FALSE(rook->isNormalMove(ROOK_COORD, Coordinates(4, 1)));
  EXPECT_THROW(board.move(ROOK_COORD, Coordinates(4, 1)), InvalidMove);
  
  EXPECT_FALSE(rook->isNormalMove(ROOK_COORD, Coordinates(2, 1)));
  EXPECT_THROW(board.move(ROOK_COORD, Coordinates(2, 1)), InvalidMove);

  EXPECT_FALSE(rook->isNormalMove(ROOK_COORD, Coordinates(5, 4)));
  EXPECT_THROW(board.move(ROOK_COORD, Coordinates(5, 4)), InvalidMove);

  EXPECT_FALSE(rook->isNormalMove(ROOK_COORD, Coordinates(5, 2)));
  EXPECT_THROW(board.move(ROOK_COORD, Coordinates(5, 2)), InvalidMove);

  EXPECT_FALSE(rook->isNormalMove(ROOK_COORD, Coordinates(1, 4)));
  EXPECT_THROW(board.move(ROOK_COORD, Coordinates(1, 4)), InvalidMove);

  EXPECT_FALSE(rook->isNormalMove(ROOK_COORD, Coordinates(1, 2)));
  EXPECT_THROW(board.move(ROOK_COORD, Coordinates(1, 2)), InvalidMove);
}