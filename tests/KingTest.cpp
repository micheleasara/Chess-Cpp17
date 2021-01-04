#include "pch.h"
#include "Board.hpp"

using Chess::King;
using Chess::Coordinates;

class KingTest : public ::testing::Test {
protected:
  Chess::Board board;
};

TEST_F(KingTest, kingCanMoveByOneForwardAndBackward) {
  board.move("E2", "E4"); board.move("E7", "E5");

  auto& whiteK = board.getPieceAtCoordinates(Coordinates(4, 0))->get();
  EXPECT_TRUE(whiteK.isMovePlausible(Coordinates(4, 0), Coordinates(4, 1)));
  EXPECT_FALSE(whiteK.isMovePlausible(Coordinates(4, 0), Coordinates(4, 2)));

  board.move("E1", "E2");
  EXPECT_TRUE(whiteK.isMovePlausible(Coordinates(4, 1), Coordinates(4, 0)));
}

TEST_F(KingTest, kingCanMoveByOneDiagonally) {
  board.move("D2", "D4"); board.move("F7", "F5");

  auto& whiteK = board.getPieceAtCoordinates(Coordinates(4, 0))->get();
  EXPECT_TRUE(whiteK.isMovePlausible(Coordinates(4, 0), Coordinates(3, 1)));
  EXPECT_FALSE(whiteK.isMovePlausible(Coordinates(4, 0), Coordinates(2, 3)));

  board.move("E1", "D2");
  auto& blackK = board.getPieceAtCoordinates(Coordinates(4, 7))->get();
  EXPECT_TRUE(blackK.isMovePlausible(Coordinates(4, 7), Coordinates(5, 6)));
  EXPECT_FALSE(blackK.isMovePlausible(Coordinates(4, 7), Coordinates(6, 5)));
}