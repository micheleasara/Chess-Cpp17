#ifndef PAWN_H
#define PAWN_H

#include "Piece.hpp"

namespace Chess {
/// Represents a pawn.
class Pawn: public Piece {
  public:
  // Constructs a pawn of the specified colour and associated to a board.
  Pawn(Piece::Colour colour, Board& board);

  //! @copydoc Board::move(Pawn&,Coordinates&)
  virtual MoveResult move(Coordinates const& destination) override;

  /// Returns "Pawn".
  std::string getName() const override;

  private:
  // Checks for pawn-specific moves and returns true if the supplied move
  // is valid in the given board, false otherwise
  virtual bool isMovePlausibleSpecific(Coordinates const& source,
                               Coordinates const& destination) const override;
};
}

#endif
