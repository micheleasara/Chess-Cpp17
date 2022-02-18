#include "Queen.hpp"
#include "Board.hpp"

namespace Chess {

Queen::Queen(Colour colour, AbstractBoard& board):
                                                PromotionPiece(colour, board) {}

std::string Queen::name() const {
  return "Queen";
}

bool Queen::isNormalMoveSpecific(Coordinates const& source,
                               Coordinates const& destination) const {
  if (source.sameRowAs(destination)) {
    return getBoard().isFreeRow(source, destination.column);
  }

  if (source.sameColumnAs(destination)) {
    return getBoard().isFreeColumn(source, destination.row);
  }

  if (source.sameDiagonalAs(destination)) {
    return getBoard().isDiagonalFree(source, destination);
  }
  return false;
}

MoveResult Queen::move(Coordinates const& source, 
                       Coordinates const& destination) {
  return getBoard().move(*this, source, destination);
}

}
