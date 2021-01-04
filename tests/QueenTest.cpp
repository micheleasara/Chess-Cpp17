#include "pch.h"
#include "Board.hpp"

using Chess::Queen;
using Chess::Coordinates;

class QueenTest : public ::testing::Test {
protected:
  Chess::Board board;
};

TEST_F(QueenTest, queenCanMoveDiagonally) {
  board.move("E2", "E3"); board.move("C7", "C6");

  auto& whiteQ = board.getPieceAtCoordinates(Coordinates(3, 0))->get();
  EXPECT_TRUE(whiteQ.isMovePlausible(Coordinates(3, 0), Coordinates(7, 4)));
  board.move("D1", "H5");
  EXPECT_TRUE(whiteQ.isMovePlausible(Coordinates(7, 4), Coordinates(3, 0)));

  auto& blackQ = board.getPieceAtCoordinates(Coordinates(3, 7))->get();
  EXPECT_TRUE(blackQ.isMovePlausible(Coordinates(3, 7), Coordinates(0, 4)));
  board.move("D8", "A5");
}

TEST_F(QueenTest, queenCanMoveForwardAndBackward) {
  board.move("D2", "D4"); board.move("D7", "D5");

  auto& whiteQ = board.getPieceAtCoordinates(Coordinates(3, 0))->get();
  EXPECT_TRUE(whiteQ.isMovePlausible(Coordinates(3, 0), Coordinates(3, 2)));
  board.move("D1", "D3");
  EXPECT_TRUE(whiteQ.isMovePlausible(Coordinates(3, 2), Coordinates(3, 0)));
}