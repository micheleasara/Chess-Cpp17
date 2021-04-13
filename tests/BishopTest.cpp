#include "pch.h"
#include "Board.hpp"

using Chess::Piece;
using Chess::Coordinates;
using Chess::Board;
using Chess::MoveResult;
using Chess::InvalidMove;

static auto constexpr BISHOP_COORD = Coordinates(3, 3);

class BishopTest : public ::testing::Test {
protected:
  Board board = Board({Coordinates(7, 6)}, {}, {}, {BISHOP_COORD}, {}, 
                    Coordinates(5, 1), {}, {}, {}, {}, {}, Coordinates(4, 7));
  Piece const* bishop = nullptr;

  void SetUp() {
    auto bishopOpt = board.at(BISHOP_COORD);
    ASSERT_TRUE(bishopOpt.has_value());
    bishop = &(bishopOpt->get());
    ASSERT_FALSE(board.isGameOver());
  }
};

TEST_F(BishopTest, canMoveDiagonallyForward) {
  EXPECT_TRUE(bishop->isMovePlausible(BISHOP_COORD, Coordinates(6, 6)));
  EXPECT_NO_THROW(board.move(BISHOP_COORD, Coordinates(6, 6)));
}

TEST_F(BishopTest, canMoveDiagonallyBackward) {
  EXPECT_TRUE(bishop->isMovePlausible(BISHOP_COORD, Coordinates(0, 0)));
  EXPECT_NO_THROW(board.move(BISHOP_COORD, Coordinates(0, 0)));
}

TEST_F(BishopTest, cannotMoveInStraightLines) {
  EXPECT_FALSE(bishop->isMovePlausible(BISHOP_COORD, Coordinates(3, 5)));
  EXPECT_THROW(board.move(BISHOP_COORD, Coordinates(3, 5)), InvalidMove);

  EXPECT_FALSE(bishop->isMovePlausible(BISHOP_COORD, Coordinates(3, 1)));
  EXPECT_THROW(board.move(BISHOP_COORD, Coordinates(3, 1)), InvalidMove);

  EXPECT_FALSE(bishop->isMovePlausible(BISHOP_COORD, Coordinates(5, 3)));
  EXPECT_THROW(board.move(BISHOP_COORD, Coordinates(5, 3)), InvalidMove);

  EXPECT_FALSE(bishop->isMovePlausible(BISHOP_COORD, Coordinates(1, 3)));
  EXPECT_THROW(board.move(BISHOP_COORD, Coordinates(1, 3)), InvalidMove);
}

TEST_F(BishopTest, cannotMoveInLShape) {
  EXPECT_FALSE(bishop->isMovePlausible(BISHOP_COORD, Coordinates(4, 5)));
  EXPECT_THROW(board.move(BISHOP_COORD, Coordinates(4, 5)), InvalidMove);

  EXPECT_FALSE(bishop->isMovePlausible(BISHOP_COORD, Coordinates(2, 5)));
  EXPECT_THROW(board.move(BISHOP_COORD, Coordinates(2, 5)), InvalidMove);

  EXPECT_FALSE(bishop->isMovePlausible(BISHOP_COORD, Coordinates(4, 1)));
  EXPECT_THROW(board.move(BISHOP_COORD, Coordinates(4, 1)), InvalidMove);
  
  EXPECT_FALSE(bishop->isMovePlausible(BISHOP_COORD, Coordinates(2, 1)));
  EXPECT_THROW(board.move(BISHOP_COORD, Coordinates(2, 1)), InvalidMove);

  EXPECT_FALSE(bishop->isMovePlausible(BISHOP_COORD, Coordinates(5, 4)));
  EXPECT_THROW(board.move(BISHOP_COORD, Coordinates(5, 4)), InvalidMove);

  EXPECT_FALSE(bishop->isMovePlausible(BISHOP_COORD, Coordinates(5, 2)));
  EXPECT_THROW(board.move(BISHOP_COORD, Coordinates(5, 2)), InvalidMove);

  EXPECT_FALSE(bishop->isMovePlausible(BISHOP_COORD, Coordinates(1, 4)));
  EXPECT_THROW(board.move(BISHOP_COORD, Coordinates(1, 4)), InvalidMove);

  EXPECT_FALSE(bishop->isMovePlausible(BISHOP_COORD, Coordinates(1, 2)));
  EXPECT_THROW(board.move(BISHOP_COORD, Coordinates(1, 2)), InvalidMove);
}