#ifndef PAST_MOVE_H
#define PAST_MOVE_H
#include <memory>
#include "Piece.hpp"

namespace Chess {

/// Contains information about a move that has already happened.
class PastMove {
public:
  PastMove(Coordinates const& source, Coordinates const& destination,
       bool sourceMoved, Piece::Colour player,
       std::unique_ptr<Piece> eatenPiece = nullptr);
  PastMove(Coordinates const& source, Coordinates const& destination,
            bool sourceMoved, Piece::Colour player,
            std::unique_ptr<Piece> eatenPiece, Coordinates const& eatenCoords);

  /// The source of the moved piece. Equal to destination in case of promotion.
  Coordinates const& source() const;
  /// The destination of the moved piece. Equal to source in case of promotion.
  Coordinates const& destination() const;
  /// True if the moved piece had moved before this move, false otherwise.
  bool sourceMovedStatus() const;
  /// Colour of the player who did the move.
  Piece::Colour const& playerColour() const;
  /// True if a piece was removed in this move (e.g. captured), false otherwise.
  bool wasAPieceRemoved() const;
  /// Transfers ownership of the removed piece, if one was indeed removed.
  std::unique_ptr<Piece> transferRemovedPiece();
  /// Returns a reference to the removed piece. Throws if no piece was removed.
  Piece const& getRemovedPiece() const;
  /// The coordinates of the removed piece.
  Coordinates const& removedPieceCoords() const;

private:
  Coordinates m_source, m_destination;
  bool m_sourceMovedStatus;
  Coordinates m_removedPieceCoords;
  Piece::Colour m_playerColour;
  std::unique_ptr<Piece> capturedPiece;
};
}

#endif // PAST_MOVE_H
