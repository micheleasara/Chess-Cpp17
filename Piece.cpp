#include "Piece.hpp"
#include "Board.hpp"
#include <iostream>

namespace Chess {
Piece::Piece(Colour colour, Board& board) : m_colour(colour),
                                                       m_board(board) {};

Colour Piece::getColour() const {
  return m_colour;
}

void Piece::setBoard(Board& board) {
  if (board.getPieceCoordinates(*this) == std::nullopt) {
    throw std::invalid_argument("A piece cannot be assigned to a board which "
                                "does not contain it");
  }

  m_board = board;
}

std::ostream& operator<<(std::ostream& out, const Piece& piece) {
  std::string owner = (piece.m_colour == Colour::White)? "White" : "Black";
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

  OptionalRef<Piece> target = getBoard().getPieceAtCoordinates(destination);
  if (target && target->get().getColour() == getColour()) {
    return false;
  }

  OptionalRef<Piece> thisPiece = getBoard().getPieceAtCoordinates(source);
  if (!thisPiece || &(thisPiece->get()) != this) {
    return false;
  }

  return isMovePlausibleSpecific(source, destination);
}

void Piece::setMovedStatus(bool moved) {
  this->m_moved = moved;
}

bool Piece::getMovedStatus() const noexcept {
  return m_moved;
}

Board& Piece::getBoard() const {
  return m_board.get();
}

}
