#include "pch.h"
#include "Board.hpp"

using Chess::Pawn;
using Chess::Coordinates;

class PawnTest : public ::testing::Test {
protected:
  Chess::Board board;
};

TEST_F(PawnTest, pawnCanDoubleStepOnFirstMove) {
  auto& wPawn = board.getPieceAtCoordinates(Coordinates(4, 1))->get();
  EXPECT_TRUE(wPawn.isMovePlausible(Coordinates(4, 1), Coordinates(4, 3)));
}

TEST_F(PawnTest, pawnCannotDoubleStepAfterFirstMove) {
  board.move("E2", "E3"); board.move("E7", "E6");
  auto& wPawn = board.getPieceAtCoordinates(Coordinates(4, 2))->get();
  EXPECT_FALSE(wPawn.isMovePlausible(Coordinates(4, 2), Coordinates(4, 4)));
}

TEST_F(PawnTest, pawnCannotMoveBackward) {
  board.move("E2", "E3"); board.move("E7", "E6");
  auto& wPawn = board.getPieceAtCoordinates(Coordinates(4, 2))->get();
  EXPECT_FALSE(wPawn.isMovePlausible(Coordinates(4, 2), Coordinates(4, 1)));
}

TEST_F(PawnTest, pawnCannotMoveDiagonallyWithoutCapturing) {
  auto& wPawn = board.getPieceAtCoordinates(Coordinates(4, 1))->get();
  EXPECT_FALSE(wPawn.isMovePlausible(Coordinates(4, 1), Coordinates(3, 2)));
  EXPECT_FALSE(wPawn.isMovePlausible(Coordinates(4, 1), Coordinates(2, 3)));
}

TEST_F(PawnTest, pawnCanMoveDiagonallyWhenCapturing) {
  auto& wPawn = board.getPieceAtCoordinates(Coordinates(4, 1))->get();
  board.move("E2", "E4"); board.move("D7", "D5");
  EXPECT_TRUE(wPawn.isMovePlausible(Coordinates(4, 3), Coordinates(3, 4)));
}

TEST_F(PawnTest, whitePawnCanEnPassant) {
  auto& wPawn = board.getPieceAtCoordinates(Coordinates(4, 1))->get();
  board.move("E2", "E4"); board.move("H7", "H5");
  board.move("E4", "E5"); board.move("D7", "D5");
  EXPECT_TRUE(wPawn.isMovePlausible(Coordinates(4, 4), Coordinates(3, 5)));
}

TEST_F(PawnTest, whitePawnCannotEnPassantAfterOneTurn) {
  auto& wPawn = board.getPieceAtCoordinates(Coordinates(4, 1))->get();
  board.move("E2", "E4"); board.move("H7", "H5");
  board.move("E4", "E5"); board.move("D7", "D5");
  EXPECT_TRUE(wPawn.isMovePlausible(Coordinates(4, 4), Coordinates(3, 5)));

  board.move("H2", "H3"); board.move("A7", "A6");
  EXPECT_FALSE(wPawn.isMovePlausible(Coordinates(4, 4), Coordinates(3, 5)));
}

TEST_F(PawnTest, blackPawnCanEnPassant) {
  auto& bPawn = board.getPieceAtCoordinates(Coordinates(4, 6))->get();
  board.move("H2", "H3"); board.move("E7", "E5");
  board.move("A2", "A3"); board.move("E5", "E4");
  board.move("D2", "D4");
  EXPECT_TRUE(bPawn.isMovePlausible(Coordinates(4, 3), Coordinates(3, 2)));
}

TEST_F(PawnTest, blackPawnCannotEnPassantAfterOneTurn) {
  auto& bPawn = board.getPieceAtCoordinates(Coordinates(4, 6))->get();
  board.move("H2", "H3"); board.move("E7", "E5");
  board.move("A2", "A3"); board.move("E5", "E4");
  board.move("D2", "D4");
  EXPECT_TRUE(bPawn.isMovePlausible(Coordinates(4, 3), Coordinates(3, 2)));

  board.move("H7", "H6");
  EXPECT_FALSE(bPawn.isMovePlausible(Coordinates(4, 3), Coordinates(3, 2)));
}