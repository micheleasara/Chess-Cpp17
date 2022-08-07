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
  /// Defines the minimum column in a human readable format.
  static char constexpr MIN_COLUMN = 'A';
  /// Defines the minimum row in a human readable format.
  static char constexpr MIN_ROW = '1';
  /// Defines the maximum column in a human readable format.
  static char constexpr MAX_COLUMN = 'H';
  /// Defines the maximum row in a human readable format.
  static char constexpr MAX_ROW = '8';
  /// Defines the maximum row number starting to count from 0.
  static int constexpr MAX_ROW_NUM = static_cast<int>(MAX_ROW - MIN_ROW);
  /// Defines the maximum column number starting to count from 0.
  static int constexpr MAX_COL_NUM = static_cast<int>(MAX_COLUMN - MIN_COLUMN);
  /// Defines the number of squares on the board.
  static int constexpr AREA = (MAX_COL_NUM + 1) * (MAX_ROW_NUM + 1);

  /// Checks if the coordinates are within a chessboard.
  static bool areWithinLimits(Coordinates const& coord);
  
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