#include "Queen.hpp"
#include "Board.hpp"

namespace Chess {
Queen::Queen(Colour colour, Board& board) :
                                            PromotionPiece(colour, board){}

std::string Queen::getName() const{
  return "Queen";
}

bool Queen::isMovePlausibleSpecific(Coordinates const& source,
                               Coordinates const& destination) const {
  // check for row and column
  if (Board::areInSameRow(source, destination))
    return board.isRowFree(source, destination.column);
  if (Board::areInSameColumn(source, destination))
    return board.isColumnFree(source, destination.row);

  // check for diagonal
  try{
    return board.isDiagonalFree(source, destination);
  // exception means source and destination are not in the same diagonal
  } catch (std::invalid_argument const&){
    return false;
  }
}

MoveResult Queen::move(Coordinates const& source, 
                       Coordinates const& destination) {
  return board.move(*this, source, destination);
}

}
