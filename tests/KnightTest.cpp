#include "pch.h"
#include "Board.hpp"

using Chess::Knight;
using Chess::Coordinates;

class KnightTest : public ::testing::Test {
protected:
  Chess::Board board;
};

TEST_F(KnightTest, knightCanMoveInLShapeWhileSteppingOverOtherPieces) {
  auto& wKnight = board.getPieceAtCoordinates(Coordinates(1, 0))->get();
  EXPECT_TRUE(wKnight.isMovePlausible(Coordinates(1, 0), Coordinates(2, 2)));
  EXPECT_TRUE(wKnight.isMovePlausible(Coordinates(1, 0), Coordinates(0, 2)));

  board.move("B1", "C3");
  EXPECT_TRUE(wKnight.isMovePlausible(Coordinates(2, 2), Coordinates(4, 3)));
  EXPECT_TRUE(wKnight.isMovePlausible(Coordinates(2, 2), Coordinates(0, 3)));
}

TEST_F(KnightTest, knightCanMoveBackwards) {
  auto& wKnight = board.getPieceAtCoordinates(Coordinates(1, 0))->get();
  board.move("B1", "C3");
  EXPECT_TRUE(wKnight.isMovePlausible(Coordinates(2, 2), Coordinates(1, 0)));
}