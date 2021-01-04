#include "pch.h"
#include "Board.hpp"

using Chess::Bishop;
using Chess::Coordinates;

class BishopTest : public ::testing::Test {
protected:
  Chess::Board board;
};

TEST_F(BishopTest, bishopCanMoveDiagonally) {
  board.move("D2", "D3"); board.move("E7", "E6");

  auto& whiteB = board.getPieceAtCoordinates(Coordinates(2, 0))->get();
  EXPECT_TRUE(whiteB.isMovePlausible(Coordinates(2, 0), Coordinates(7, 5)));
  board.move("C1", "H6");
  EXPECT_TRUE(whiteB.isMovePlausible(Coordinates(7, 5), Coordinates(2, 0)));

  auto& blackB = board.getPieceAtCoordinates(Coordinates(5, 7))->get();
  EXPECT_TRUE(blackB.isMovePlausible(Coordinates(5, 7), Coordinates(0, 2)));
}