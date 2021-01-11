#ifndef KNIGHT_H
#define KNIGHT_H

#include "Piece.hpp"

namespace Chess {
/// Represents a knight.
class Knight: public PromotionPiece {
  public:
  /// Constructs a knight of the given colour and associated to a board.
  Knight(Piece::Colour colour, Board& board);

  //! @copydoc Board::move(Pawn&,Coordinates const&,Coordinates const&)
  virtual MoveResult move(Coordinates const& source,
                          Coordinates const& destination) override;

  // Returns "Knight"
  std::string getName() const override;

  private:
  // Checks for knight-specific moves and returns true if the given move
  // is valid, false otherwise.
  virtual bool isMovePlausibleSpecific(Coordinates const& source,
                               Coordinates const& destination) const override;
};
}
#endif
