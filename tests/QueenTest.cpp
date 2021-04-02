#include "pch.h"
#include "Board.hpp"

using Chess::Piece;
using Chess::Coordinates;
using Chess::Board;
using Chess::MoveResult;
using Chess::InvalidMove;

static auto constexpr QUEEN_COORD = Coordinates(3,3);

class QueenTest : public ::testing::Test {
protected:
  Board board = Board({}, {}, {}, {}, {QUEEN_COORD}, Coordinates(5,1), 
                      {}, {}, {}, {}, {}, Coordinates(5,7));
  Piece* queen = nullptr;

  void SetUp() {
    auto queenOpt = board.getPieceAtCoordinates(QUEEN_COORD);
    ASSERT_TRUE(queenOpt.has_value());
    queen = &(queenOpt->get());
    ASSERT_FALSE(board.isGameOver());
  }
};

TEST_F(QueenTest, canMoveDiagonallyForward) {
  EXPECT_TRUE(queen->isMovePlausible(QUEEN_COORD, Coordinates(6, 6)));
  EXPECT_NO_THROW(queen->move(QUEEN_COORD, Coordinates(6, 6)));
}

TEST_F(QueenTest, canMoveDiagonallyBackward) {
  EXPECT_TRUE(queen->isMovePlausible(QUEEN_COORD, Coordinates(0, 0)));
  EXPECT_NO_THROW(queen->move(QUEEN_COORD, Coordinates(0, 0)));
}

TEST_F(QueenTest, canMoveBackwardVertically) {
  EXPECT_TRUE(queen->isMovePlausible(QUEEN_COORD, Coordinates(3, 1)));
  EXPECT_NO_THROW(queen->move(QUEEN_COORD, Coordinates(3, 1)));
}

TEST_F(QueenTest, canMoveForwardVertically) {
  EXPECT_TRUE(queen->isMovePlausible(QUEEN_COORD, Coordinates(3, 5)));
  EXPECT_NO_THROW(queen->move(QUEEN_COORD, Coordinates(3, 5)));
}

TEST_F(QueenTest, canMoveBackwardHorizontally) {
  EXPECT_TRUE(queen->isMovePlausible(QUEEN_COORD, Coordinates(1, 3)));
  EXPECT_NO_THROW(queen->move(QUEEN_COORD, Coordinates(1, 3)));
}

TEST_F(QueenTest, canMoveForwardHorizontally) {
  EXPECT_TRUE(queen->isMovePlausible(QUEEN_COORD, Coordinates(5, 3)));
  EXPECT_NO_THROW(queen->move(QUEEN_COORD, Coordinates(5, 3)));
}

TEST_F(QueenTest, cannotMoveInLShape) {
  EXPECT_FALSE(queen->isMovePlausible(QUEEN_COORD, Coordinates(4, 5)));
  EXPECT_THROW(queen->move(QUEEN_COORD, Coordinates(4, 5)), InvalidMove);

  EXPECT_FALSE(queen->isMovePlausible(QUEEN_COORD, Coordinates(2, 5)));
  EXPECT_THROW(queen->move(QUEEN_COORD, Coordinates(2, 5)), InvalidMove);

  EXPECT_FALSE(queen->isMovePlausible(QUEEN_COORD, Coordinates(4, 1)));
  EXPECT_THROW(queen->move(QUEEN_COORD, Coordinates(4, 1)), InvalidMove);
  
  EXPECT_FALSE(queen->isMovePlausible(QUEEN_COORD, Coordinates(2, 1)));
  EXPECT_THROW(queen->move(QUEEN_COORD, Coordinates(2, 1)), InvalidMove);

  EXPECT_FALSE(queen->isMovePlausible(QUEEN_COORD, Coordinates(5, 4)));
  EXPECT_THROW(queen->move(QUEEN_COORD, Coordinates(5, 4)), InvalidMove);

  EXPECT_FALSE(queen->isMovePlausible(QUEEN_COORD, Coordinates(5, 2)));
  EXPECT_THROW(queen->move(QUEEN_COORD, Coordinates(5, 2)), InvalidMove);

  EXPECT_FALSE(queen->isMovePlausible(QUEEN_COORD, Coordinates(1, 4)));
  EXPECT_THROW(queen->move(QUEEN_COORD, Coordinates(1, 4)), InvalidMove);

  EXPECT_FALSE(queen->isMovePlausible(QUEEN_COORD, Coordinates(1, 2)));
  EXPECT_THROW(queen->move(QUEEN_COORD, Coordinates(1, 2)), InvalidMove);
}