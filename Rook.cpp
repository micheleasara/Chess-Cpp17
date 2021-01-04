#include "Rook.hpp"
#include "Board.hpp"

namespace Chess {


Rook::Rook(Piece::Colour colour, Board& board) :
                                              PromotionPiece(colour, board){}

bool Rook::isMovePlausibleSpecific(Coordinates const& source,
                               Coordinates const& destination) const {

  if (Board::areInSameRow(source, destination))
      return board.isRowFree(source, destination.column);
  else if (Board::areInSameColumn(source, destination))
      return board.isColumnFree(source, destination.row);

  // return false if destination is not in same row or column
  return false;
}

std::string Rook::getName() const{
  return "Rook";
}

MoveResult Rook::move(Coordinates const& destination) {
  return board.move(*this, destination);
}
}
