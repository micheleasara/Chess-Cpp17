#include "King.hpp"
#include "Board.hpp"

namespace Chess {

King::King(Colour colour, AbstractBoard& board): Piece(colour, board) {}

std::string King::name() const {
  return "King";
}

bool King::isNormalMoveSpecific(Coordinates const& source,
                               Coordinates const& destination) const {
  int colDiff = abs(source.column - destination.column);
  int rowDiff = abs(source.row - destination.row);

  if (colDiff + rowDiff > 2) {
    return false;
  }
  if (colDiff != 1 && rowDiff != 1) {
    return false;
  }

  return true;
}

MoveResult King::move(Coordinates const& source,
                      Coordinates const& destination) {
  return getBoard().move(*this, source, destination);
}

}
