#include "Rook.hpp"
#include "Board.hpp"

namespace Chess {

std::vector<Coordinates> const Rook::WHITE_STD_INIT = 
                                         {Coordinates(0, 0), Coordinates(7, 0)};
std::vector<Coordinates> const Rook::BLACK_STD_INIT = 
                                         {Coordinates(0, 7), Coordinates(7, 7)};

Rook::Rook(Colour colour, Board& board): PromotionPiece(colour, board) {}

bool Rook::isMovePlausibleSpecific(Coordinates const& source,
                                   Coordinates const& destination) const {
  if (Board::areInSameRow(source, destination)) {
      return getBoard().isRowFree(source, destination.column);
  }
  else if (Board::areInSameColumn(source, destination)) {
      return getBoard().isColumnFree(source, destination.row);
  }

  return false;
}

std::string Rook::getName() const {
  return "Rook";
}

MoveResult Rook::move(Coordinates const& source, 
                      Coordinates const& destination) {
  return getBoard().move(*this, source, destination);
}
}
