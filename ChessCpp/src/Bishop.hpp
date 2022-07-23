#ifndef BISHOP_H
#define BISHOP_H

#include "Piece.hpp"

namespace Chess {

/// Represents a bishop.
class Bishop final: public PromotionPiece {
  public:
  /**
   Defines the standard starting positions of the white bishops on a board.
   Each coordinate pair is given in order of increasing columns.
  */
  static std::vector<Coordinates> const WHITE_STD_INIT;

  /**
   Defines the standard starting positions of the black bishops on a board.
   Each coordinate pair is given in order of increasing columns.
  */
  static std::vector<Coordinates> const BLACK_STD_INIT;

  /// Constructs a bishop of the given colour and associated to a board.
  Bishop(Colour colour, AbstractBoard& board);

  //! @copydoc AbstractBoard::move(Pawn&,Coordinates const&,Coordinates const&)
  MoveResult move(Coordinates const& source, 
                          Coordinates const& destination) override;

  /// Returns "Bishop".
  std::string name() const override;

  private:
  /**
   Checks for bishop-specific moves and returns true if the given move
   is valid in the associated board.
  */
  bool isNormalMoveSpecific(Coordinates const& source,
                           Coordinates const& destination) const override;
};

}

#endif
