#ifndef QUEEN_H
#define QUEEN_H

#include "Piece.hpp"

namespace Chess {
// Represents a queen.
class Queen: public PromotionPiece {
  public:
  /// Constructs a queen of the given colour and associated to a board.
  Queen(Colour colour, Board& board);

  //! @copydoc Board::move(Pawn&,Coordinates const&,Coordinates const&)
  virtual MoveResult move(Coordinates const& source,
                          Coordinates const& destination) override;

  /// Returns "Queen".
  std::string getName() const override;

  private:
  /**
   Checks for queen-specific moves and returns true if the supplied move
   is valid in the associated board, false otherwise.
  */
  bool isMovePlausibleSpecific(Coordinates const& source,
                           Coordinates const& destination) const override final;
};
}

#endif
