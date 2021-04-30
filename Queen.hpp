#ifndef QUEEN_H
#define QUEEN_H

#include "Piece.hpp"

namespace Chess {

// Represents a queen.
class Queen final: public PromotionPiece {
public:
  /// Defines the standard starting position of the white queen on a board.
  static auto constexpr WHITE_STD_INIT = Coordinates(3, 0);

  /// Defines the standard starting position of the black queen on a board.
  static auto constexpr BLACK_STD_INIT = Coordinates(3, 7);

  /// Constructs a queen of the given colour and associated to a board.
  Queen(Colour colour, Board& board);

  //! @copydoc Board::move(Pawn&,Coordinates const&,Coordinates const&)
  MoveResult move(Coordinates const& source,
                          Coordinates const& destination) override;

  /// Returns "Queen".
  std::string name() const override;

private:
  /**
   Checks for queen-specific moves and returns true if the supplied move
   is valid in the associated board, false otherwise.
  */
  bool isNormalMoveSpecific(Coordinates const& source,
                           Coordinates const& destination) const override;
};

}

#endif
