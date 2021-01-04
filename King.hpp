#ifndef KING_H
#define KING_H

#include "Piece.hpp"

namespace Chess {
/// Represents a king.
class King: public Piece {
  public:
  /// Constructs a king of the given colour and associated to a board.
  King(Piece::Colour colour, Board& board);

  //! @copydoc Board::move(std::string,std::string)
  virtual MoveResult move(Coordinates const& destination) override;

  /// Returns "King".
  std::string getName() const override;

  private:

  // Checks for king-specific moves and returns true if the supplied move
  // is valid, false otherwise.
  // This function does not handle castling, as the Board is
  // considered responsible for it
  virtual bool isMovePlausibleSpecific(Coordinates const& source,
                               Coordinates const& destination) const override;
};
}

#endif
