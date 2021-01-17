#include "PastMove.hpp"

namespace Chess {
PastMove::PastMove(Coordinates const& source, Coordinates const& destination,
        bool sourceMoved, Piece::Colour player,
        std::unique_ptr<Piece> capturedPiece):
                              PastMove(source, destination, sourceMoved, player,
                                       std::move(capturedPiece), destination) {}

PastMove::PastMove(Coordinates const& source, Coordinates const& destination,
       bool sourceMoved, Piece::Colour player,
       std::unique_ptr<Piece> eatenPiece, Coordinates const& eatenCoords):
       m_source(source), m_destination(destination),
       m_sourceMovedStatus(sourceMoved), m_removedPieceCoords(eatenCoords),
       m_playerColour(player), capturedPiece(std::move(eatenPiece)) {}

Coordinates const& PastMove::source() const {
  return m_source;
}

Coordinates const& PastMove::destination() const {
  return m_destination;
}

Coordinates const& PastMove::removedPieceCoords() const {
  return m_removedPieceCoords;
}

bool PastMove::sourceMovedStatus() const {
  return m_sourceMovedStatus;
}

Piece::Colour const& PastMove::playerColour() const {
  return m_playerColour;
}

bool PastMove::wasAPieceRemoved() const {
  return capturedPiece != nullptr;
}

std::unique_ptr<Piece> PastMove::transferRemovedPiece() {
  return std::move(capturedPiece);
}

Piece const& PastMove::getRemovedPiece() const {
  if (capturedPiece) {
    return *capturedPiece;
  }
  throw std::logic_error("No piece was removed in this past move record");
}
}
