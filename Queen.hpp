#ifndef QUEEN_H
#define QUEEN_H

#include "Piece.hpp"

namespace Chess {
// Represents a queen.
class Queen: public PromotionPiece {
  public:
  /// Constructs a queen of the given colour and associated to a board.
  Queen(Piece::Colour colour, Board& board);

  //! @copydoc Board::move(Pawn&,Coordinates&)
  virtual MoveResult move(Coordinates const& destination) override;

  /// Returns "Queen".
  std::string getName() const override;

  private:
  // Checks for queen-specific moves and returns true if the supplied move
  // is valid in the given board, false otherwise
  virtual bool isMovePlausibleSpecific(Coordinates const& source,
                               Coordinates const& destination) const override;
};
}

#endif
