#include "Knight.hpp"
#include "Board.hpp"
#include <cmath>

namespace Chess {

std::vector<Coordinates> const Knight::WHITE_STD_INIT = 
                                        {Coordinates(1, 0), Coordinates(6, 0)};
std::vector<Coordinates> const Knight::BLACK_STD_INIT = 
                                        {Coordinates(1, 7), Coordinates(6, 7)};

Knight::Knight(Colour colour, Board& board): PromotionPiece(colour, board) {}

bool Knight::isMovePlausibleSpecific(Coordinates const& source,
                               Coordinates const& destination) const {
  int rowDiff = abs(source.row - destination.row);
  int colDiff = abs(source.column - destination.column);

  // L-shape is modeled as row and column differences
  return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);
}

std::string Knight::name() const {
  return "Knight";
}

MoveResult Knight::move(Coordinates const& source,
                        Coordinates const& destination) {
  return getBoard().move(*this, source, destination);
}

}
