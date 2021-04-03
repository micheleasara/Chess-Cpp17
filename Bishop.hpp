#ifndef BISHOP_H
#define BISHOP_H

#include "Piece.hpp"

namespace Chess {
/// Represents a bishop.
class Bishop: public PromotionPiece {
  public:
  /// Constructs a bishop of the given colour and associated to a board.
  Bishop(Colour colour, Board& board);

  //! @copydoc Board::move(Pawn&,Coordinates const&,Coordinates const&)
  virtual MoveResult move(Coordinates const& source, 
                          Coordinates const& destination) override;

  /// Returns "Bishop".
  std::string getName() const override;

  private:
  /**
   Checks for bishop-specific moves and returns true if the given move
   is valid in the associated board.
  */
  bool isMovePlausibleSpecific(Coordinates const& source,
                           Coordinates const& destination) const override final;
};
}

#endif
