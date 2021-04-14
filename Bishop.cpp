#include "Bishop.hpp"
#include "Board.hpp"

namespace Chess {

std::vector<Coordinates> const Bishop::WHITE_STD_INIT = 
                                         {Coordinates(2, 0), Coordinates(5, 0)};
std::vector<Coordinates> const Bishop::BLACK_STD_INIT =
                                         {Coordinates(2, 7), Coordinates(5, 7)};

Bishop::Bishop(Colour colour, Board& board): PromotionPiece(colour, board) {}

std::string Bishop::name() const {
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
