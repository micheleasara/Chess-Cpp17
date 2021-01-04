#include "pch.h"
#include "Board.hpp"

using Chess::Rook;
using Chess::Coordinates;

class RookTest : public ::testing::Test {
protected:
  Chess::Board board;
};

TEST_F(RookTest, rookCanMoveForwardAndBackward) {
  board.move("A2", "A4"); board.move("A7", "A5");

  auto& whiteR = board.getPieceAtCoordinates(Coordinates(0, 0))->get();
  EXPECT_TRUE(whiteR.isMovePlausible(Coordinates(0, 0), Coordinates(0, 2)));
  board.move("A1", "A3");
  EXPECT_TRUE(whiteR.isMovePlausible(Coordinates(0, 2), Coordinates(0, 0)));
}