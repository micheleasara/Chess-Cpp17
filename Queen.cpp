#include "Queen.hpp"
#include "Board.hpp"

namespace Chess {

Queen::Queen(Colour colour, Board& board): PromotionPiece(colour, board) {}

std::string Queen::name() const {
  return "Queen";
}

bool Queen::isNormalMoveSpecific(Coordinates const& source,
                               Coordinates const& destination) const {
  if (Board::areInSameRow(source, destination)) {
    return getBoard().isFreeRow(source, destination.column);
  }

  if (Board::areInSameColumn(source, destination)) {
    return getBoard().isFreeColumn(source, destination.row);
  }

  if (getBoard().areInSameDiagonal(source, destination)) {
    return getBoard().isDiagonalFree(source, destination);
  }
  return false;
}

MoveResult Queen::move(Coordinates const& source, 
                       Coordinates const& destination) {
  return getBoard().move(*this, source, destination);
}

}
