#include "Bishop.hpp"
#include "Board.hpp"

namespace Chess {
Bishop::Bishop(Colour colour, Board& board):
                                                PromotionPiece(colour, board){}

std::string Bishop::getName() const{
  return "Bishop";
}

bool Bishop::isMovePlausibleSpecific(Coordinates const& source,
                               Coordinates const& destination) const {
  try {
    return getBoard().isDiagonalFree(source, destination);
  // exception means source and destination are not in the same diagonal
  } catch (std::invalid_argument const&) {
    return false;
  }
}

MoveResult Bishop::move(Coordinates const& source, 
                        Coordinates const& destination) {
  return getBoard().move(*this, source, destination);
}
}
