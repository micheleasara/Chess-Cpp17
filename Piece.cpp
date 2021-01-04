#include "Piece.hpp"
#include "Board.hpp"
#include <iostream>

namespace Chess {
Piece::Piece(Piece::Colour colour, Board& board) : colour(colour),
                                                       board(board) {};

Piece::Colour Piece::getColour() const {
  return colour;
}

std::ostream& operator<<(std::ostream& out, const Piece& piece) {
  std::string owner = (piece.colour == Piece::Colour::White)? "White" : "Black";
  return out << (owner + "'s " + piece.getName());
}

bool Piece::isMovePlausible(Coordinates const& source,
                            Coordinates const& destination) const {
  if (!Board::areWithinLimits(source) ||
      !Board::areWithinLimits(destination)) {
    return false;
  }

  if (source == destination) {
    return false;
  }

  OptionalRef<Piece> target = board.getPieceAtCoordinates(destination);
  if (target && target->get().getColour() == getColour()) {
    return false;
  }

  OptionalRef<Piece> thisPiece = board.getPieceAtCoordinates(source);
  if (!thisPiece || &(thisPiece->get()) != this) {
    return false;
  }

  return isMovePlausibleSpecific(source, destination);
}

void Piece::setMovedStatus(bool moved) {
  this->moved = moved;
}

bool Piece::getMovedStatus() const noexcept {
  return moved;
}
}
