#ifndef PAWN_H
#define PAWN_H

#include "Piece.hpp"

namespace Chess {

/// Represents a pawn.
class Pawn final: public Piece {
public:
  /**
   Defines the standard starting positions of the white pawns on a board.
   Each coordinate pair is given in order of increasing columns.
  */
  static std::vector<Coordinates> const WHITE_STD_INIT;

  /**
   Defines the standard starting positions of the black pawns on a board.
   Each coordinate pair is given in order of increasing columns.
  */
  static std::vector<Coordinates> const BLACK_STD_INIT;

  // Constructs a pawn of the specified colour and associated to a board.
  Pawn(Colour colour, AbstractBoard& board);

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
  bool isNormalMoveSpecific(Coordinates const& source,
                           Coordinates const& destination) const override;
};

}

#endif
