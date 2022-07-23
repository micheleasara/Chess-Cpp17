#include "AbstractBoard.hpp"
#include "Rook.hpp"

namespace Chess {

std::vector<Coordinates> const Rook::WHITE_STD_INIT = 
                                         {Coordinates(0, 0), Coordinates(7, 0)};
std::vector<Coordinates> const Rook::BLACK_STD_INIT = 
                                         {Coordinates(0, 7), Coordinates(7, 7)};

Rook::Rook(Colour colour, AbstractBoard& board): 
                                                PromotionPiece(colour, board) {}

bool Rook::isNormalMoveSpecific(Coordinates const& source,
                                   Coordinates const& destination) const {
  if (source.sameRowAs(destination)) {
      return getBoard().isFreeRow(source, destination.column);
  }
  else if (source.sameColumnAs(destination)) {
      return getBoard().isFreeColumn(source, destination.row);
  }

  return false;
}

std::string Rook::name() const {
  return "Rook";
}

MoveResult Rook::move(Coordinates const& source, 
                      Coordinates const& destination) {
  return getMutableBoard().move(*this, source, destination);
}
}
