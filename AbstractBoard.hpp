#ifndef ABSTRACT_BOARD_H
#define ABSTRACT_BOARD_H

namespace Chess {

class MoveResult;
class Piece; class Pawn; class PromotionPiece; class King;
struct Coordinates;

/**
  Defines an interface to enable basic communication between a piece and its
  containing board.
*/
class AbstractBoard {
public:
  /**
    Moves the pawn from the source to the destination provided, if the move is 
    legitimate.
  */
  virtual MoveResult move(Pawn& piece, Coordinates const& source,
                                       Coordinates const& destination) = 0;
  /**
    Moves the promotion piece from the source to the destination provided, if 
    the move is legitimate.
  */
  virtual MoveResult move(PromotionPiece& piece, Coordinates const& source,
                                            Coordinates const& destination) = 0;
  /**
    Moves the king from the source to the destination provided, if the move is 
    legitimate.
  */
  virtual MoveResult move(King& piece, Coordinates const& source,
                                       Coordinates const& destination) = 0;

  //! @copydoc Board::at(Coordinates const&,Coordinates const&)
  virtual Piece const* at(Coordinates const& coord) const = 0;

  //! @copydoc Board::isFreeColumn(Coordinates const&,int)
  virtual bool isFreeColumn(Coordinates const& source, int limitRow) const = 0;
  //! @copydoc Board::isFreeRow(Coordinates const&,int)
  virtual bool isFreeRow(Coordinates const& source, int limitCol) const = 0;
  //! @copydoc Board::isFreeDiagonal(Coordinates const&,Coordinates const&)
  virtual bool isFreeDiagonal(Coordinates const& source,
                                     Coordinates const& destination) const = 0;
  //! @copydoc Board::isValidEnPassant(Pawn const&,Coordinates const&,Coordinates const&)
  virtual bool isValidEnPassant(Pawn const& pawn, Coordinates const& source,
                                     Coordinates const& destination) const = 0;

  virtual ~AbstractBoard() = default;

  AbstractBoard& operator=(AbstractBoard const&) = delete;
  AbstractBoard& operator=(AbstractBoard&&) = delete;
};

}

#endif // ABSTRACT_BOARD_H