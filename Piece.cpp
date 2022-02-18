#include "Piece.hpp"
#include "Board.hpp"
#include <iostream>

namespace Chess {

Piece::Piece(Colour colour, AbstractBoard& board): m_colour(colour), 
                                                    m_board(board) {}

Colour Piece::getColour() const {
  return m_colour;
}

void Piece::setBoard(AbstractBoard& board) noexcept {
  m_board = board;
}

std::ostream& operator<<(std::ostream& out, const Piece& piece) {
  std::string owner = (piece.m_colour == Colour::White)? "White" : "Black";
  return out << (owner + "'s " + piece.name());
}

bool Piece::isNormalMove(Coordinates const& source,
                            Coordinates const& destination) const {
  if (!Board::areWithinLimits(source) || !Board::areWithinLimits(destination)) {
    return false;
  }

  if (source == destination) {
    return false;
  }

  auto target = getBoard().at(destination);
  if (target && target->getColour() == getColour()) {
    return false;
  }

  auto thisPiece = getBoard().at(source);
  if (!thisPiece || thisPiece != this) {
    return false;
  }

  return isNormalMoveSpecific(source, destination);
}

void Piece::setMovedStatus(bool moved) {
  this->m_moved = moved;
}

bool Piece::getMovedStatus() const {
  return m_moved;
}

AbstractBoard& Piece::getBoard() const {
  return m_board.get();
}

}
