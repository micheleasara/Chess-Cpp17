#include "AbstractBoard.hpp"
#include "Bishop.hpp"

namespace Chess {

std::vector<Coordinates> const Bishop::WHITE_STD_INIT = 
                                         {Coordinates(2, 0), Coordinates(5, 0)};
std::vector<Coordinates> const Bishop::BLACK_STD_INIT =
                                         {Coordinates(2, 7), Coordinates(5, 7)};

Bishop::Bishop(Colour colour, AbstractBoard& board): 
                                                PromotionPiece(colour, board) {}

std::string Bishop::name() const {
  return "Bishop";
}

bool Bishop::isNormalMoveSpecific(Coordinates const& source,
                               Coordinates const& destination) const {
  if (source.sameDiagonalAs(destination)) {
    return getBoard().isFreeDiagonal(source, destination);
  }
  return false;
}

MoveResult Bishop::move(Coordinates const& source, 
                        Coordinates const& destination) {
  return getMutableBoard().move(*this, source, destination);
}

}
