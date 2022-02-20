#include "AbstractBoard.hpp"
#include <iostream>
#include "Pawn.hpp"

namespace Chess {

Pawn::Pawn(Colour colour, AbstractBoard& board): Piece(colour, board) {}

std::string Pawn::name() const {
  return "Pawn";
}

bool Pawn::isNormalMoveSpecific(Coordinates const& source,
                               Coordinates const& destination) const {
  int forwardSteps = destination.row - source.row;
  int horizontalSteps = abs(destination.column - source.column);

  // white only moves in increasing row direction, black the opposite
  // therefore, 'forward' is relative to the colour
  forwardSteps *= (getColour() == Colour::White) ? 1 : -1;
  auto piece = getBoard().at(destination);

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
    return getBoard().isFreeColumn(source, destination.row);
  }
  return false;
}

MoveResult Pawn::move(Coordinates const& source,
                      Coordinates const& destination) {
  return getMutableBoard().move(*this, source, destination);
}

std::vector<Coordinates> const Pawn::WHITE_STD_INIT = 
                                         {Coordinates(0, 1),
                                          Coordinates(1, 1),
                                          Coordinates(2, 1),
                                          Coordinates(3, 1),
                                          Coordinates(4, 1),
                                          Coordinates(5, 1),
                                          Coordinates(6, 1),
                                          Coordinates(7, 1)};

std::vector<Coordinates> const Pawn::BLACK_STD_INIT = 
                                         {Coordinates(0, 6),
                                          Coordinates(1, 6),
                                          Coordinates(2, 6),
                                          Coordinates(3, 6),
                                          Coordinates(4, 6),
                                          Coordinates(5, 6),
                                          Coordinates(6, 6),
                                          Coordinates(7, 6)};

}
