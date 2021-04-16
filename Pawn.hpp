#ifndef PAWN_H
#define PAWN_H

#include "Piece.hpp"

namespace Chess {

/// Represents a pawn.
class Pawn final: public Piece {
public:
  // Constructs a pawn of the specified colour and associated to a board.
  Pawn(Colour colour, Board& board);

  //! @copydoc Board::move(Pawn&,Coordinates const&,Coordinates const&)
  MoveResult move(Coordinates const& source,
                          Coordinates const& destination) override;

  /// Returns "Pawn".
  std::string name() const override;

private:
  /**
   Checks for pawn-specific moves and returns true if the supplied move
   is valid in the associated board, false otherwise.
  */
  bool isMovePlausibleSpecific(Coordinates const& source,
                           Coordinates const& destination) const override;
};

}

#endif
