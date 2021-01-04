#include "Knight.hpp"
#include "Board.hpp"
#include <cmath>

namespace Chess {
Knight::Knight(Piece::Colour colour, Board& board) :
                                                PromotionPiece(colour, board){}

// third parameter 'board' is not used by knight: warning should be suppressed
bool Knight::isMovePlausibleSpecific(Coordinates const& source,
                               Coordinates const& destination) const {
  int rowDiff = abs(source.row - destination.row);
  int colDiff = abs(source.column - destination.column);

  // L-shape is modeled as row and column differences
  return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);
}

std::string Knight::getName() const{
  return "Knight";
}

MoveResult Knight::move(Coordinates const& destination) {
  return board.move(*this, destination);
}

}
