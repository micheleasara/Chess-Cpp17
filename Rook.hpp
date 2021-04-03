#ifndef ROOK_H
#define ROOK_H

#include "Piece.hpp"

namespace Chess {
/// Represents a rook.
class Rook: public PromotionPiece {
  public:
  /// Constructs a rook of the given colour and associated to a board.
  Rook(Colour colour, Board& board);

  //! @copydoc Board::move(Pawn&,Coordinates const&,Coordinates const&)
  virtual MoveResult move(Coordinates const& source,
                          Coordinates const& destination) override;

  /// Returns "Rook".
  std::string getName() const override;

  private:
  /**
   Checks for rook-specific moves and returns true if the supplied move
   is valid in the associated board, false otherwise.
  */
  bool isMovePlausibleSpecific(Coordinates const& source,
                           Coordinates const& destination) const override final;
};
}

#endif
