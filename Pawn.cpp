#include "Pawn.hpp"
#include "Board.hpp"
#include <iostream>

namespace Chess {
Pawn::Pawn(Colour colour, Board& board) : Piece(colour, board){}

std::string Pawn::getName() const {
  return "Pawn";
}

bool Pawn::isMovePlausibleSpecific(Coordinates const& source,
                               Coordinates const& destination) const {
  int forwardSteps = destination.row - source.row;
  int horizontalSteps = abs(destination.column - source.column);

  // white only moves in increasing row direction, black the opposite
  // therefore, 'forward' is relative to the colour
  forwardSteps *= (getColour() == Colour::White) ? 1 : -1;
  auto piece = getBoard().getPieceAtCoordinates(destination);

  // return true only for: normal pawn step, capturing one step diagonally,
  // double step as first move and en passant
  if (forwardSteps == 1 && horizontalSteps == 0 && !piece) {
    return true;
  }
  if (forwardSteps == 1 && horizontalSteps == 1 && piece) {
    return true;
  }
  if (getBoard().isValidEnPassant(*this, source, destination)) {
    return true;
  }
  if (forwardSteps == 2 && horizontalSteps == 0 && !piece &&
                                                        !getMovedStatus()) {
    return getBoard().isColumnFree(source, destination.row);
  }
  return false;
}

MoveResult Pawn::move(Coordinates const& source,
                      Coordinates const& destination) {
  return getBoard().move(*this, source, destination);
}
}
