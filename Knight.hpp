#ifndef KNIGHT_H
#define KNIGHT_H

#include "Piece.hpp"

namespace Chess {

/// Represents a knight.
class Knight final: public PromotionPiece {
public:
  /**
   Defines the standard starting positions of the white knights on a board.
   Each coordinate pair is given in order of increasing columns.
  */
  static std::vector<Coordinates> const WHITE_STD_INIT;

  /**
   Defines the standard starting positions of the black knights on a board.
   Each coordinate pair is given in order of increasing columns.
  */
  static std::vector<Coordinates> const BLACK_STD_INIT;

  /// Constructs a knight of the given colour and associated to a board.
  Knight(Colour colour, AbstractBoard& board);

  //! @copydoc AbstractBoard::move(Pawn&,Coordinates const&,Coordinates const&)
  MoveResult move(Coordinates const& source,
                          Coordinates const& destination) override;

  /// Returns "Knight"
  std::string name() const override;

private:
  /**
   Checks for knight-specific moves and returns true if the given move
   is valid, false otherwise.
  */
  bool isNormalMoveSpecific(Coordinates const& source,
                           Coordinates const& destination) const override;
};

}
#endif
