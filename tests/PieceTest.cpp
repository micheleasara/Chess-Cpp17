#include "pch.h"
#include "Piece.hpp"
#include "Board.hpp"

using Chess::Coordinates;
using Chess::Board;
using Chess::Colour;

class PieceStub : public Chess::Piece {
public:
  PieceStub(Colour colour, Chess::Board& board): Piece(colour, board) {}

  Chess::MoveResult move(Coordinates const& source,
                         Coordinates const& destination) override {
    return Chess::MoveResult(Chess::MoveResult::GameState::NORMAL);
  }

  std::string name() const { return ""; }

private:
  virtual bool isMovePlausibleSpecific(Coordinates const& source,
                                       Coordinates const& destination) const {
    return true;
  }
};

class PieceTest : public ::testing::Test {
protected:
  Chess::Board board;
  Chess::Piece& piece = PieceStub(Colour::White, board);
};

TEST_F(PieceTest, cannotMoveFromOutOfBoundsCoordinates) {
  EXPECT_FALSE(piece.isMovePlausible(Coordinates(-1, 0), Coordinates(0, 0)));
  EXPECT_FALSE(piece.isMovePlausible(Coordinates(0, -1), Coordinates(0, 0)));
  EXPECT_FALSE(piece.isMovePlausible(Coordinates(Board::MAX_COL_NUM+1, 0),
                                     Coordinates(0, 0)));
  EXPECT_FALSE(piece.isMovePlausible(Coordinates(0, Board::MAX_ROW_NUM+1),
                                     Coordinates(0, 0)));
}

TEST_F(PieceTest, cannotMoveToOutOfBoundsCoordinates) {
  EXPECT_FALSE(piece.isMovePlausible(Coordinates(0, 0), Coordinates(-1, 0)));
  EXPECT_FALSE(piece.isMovePlausible(Coordinates(0, 0), Coordinates(0, -1)));
  EXPECT_FALSE(piece.isMovePlausible(Coordinates(0, 0),
                                  Coordinates(Board::MAX_COL_NUM + 1, 0)));
  EXPECT_FALSE(piece.isMovePlausible(Coordinates(0, 0),
                                  Coordinates(0, Board::MAX_ROW_NUM + 1)));
}

TEST_F(PieceTest, cannotMoveIfSourceAndDestinationAreEqual) {
  EXPECT_FALSE(piece.isMovePlausible(Coordinates(1, 1), Coordinates(1, 1)));
}

TEST_F(PieceTest, cannotMoveIfNotInChessboard) {
  EXPECT_FALSE(piece.isMovePlausible(Coordinates(1, 1), Coordinates(1, 2)));
}

TEST_F(PieceTest, cannotMoveIfDestinationIsOccupiedByAlly) {
  EXPECT_FALSE(piece.isMovePlausible(Coordinates(0, 0), Coordinates(0, 1)));
}