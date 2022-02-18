#ifndef PIECE_H
#define PIECE_H

#include "MoveResult.hpp"
#include <ostream>
#include "Utils.hpp"

namespace Chess {
class AbstractBoard;

/// Represents a generic piece.
class Piece {
public:
  /// Creates a chess piece of the given colour and associated with a board.
  Piece(Colour colour, AbstractBoard& board);

  /**
   Returns whether moving from the source to the destination provided is a 
   normal move according to the piece logic and patterns. Castling and checks
   are considered responsibilities of the board, and are ignored.
  
   Returns false if certain conditions common to all pieces are not satisfied.
   For example:
   coordinates must be within the board limits, a piece cannot move to a square
   taken by an ally, and source and destination cannot match.

   In all other cases, it returns the derived class' isNormalMoveSpecific().
  */
  bool isNormalMove(Coordinates const& source,
                       Coordinates const& destination) const;

  //! @copydoc Board::move(Pawn&,Coordinates const&,Coordinates const&)
  virtual MoveResult move(Coordinates const& source, 
                          Coordinates const& destination) = 0;

  /// Flags the piece to mean it has moved before.
  void setMovedStatus(bool moved);

  /// Returns whether the piece has moved before.
  bool getMovedStatus() const;

  /// Returns the colour of the piece.
  Colour getColour() const;

  /**
   Performs one-sided assignment of the given board to this piece.
   In other words, the piece will act assuming it is contained in the board.
  */
  void setBoard(AbstractBoard& board) noexcept;

  /// Returns the board currently associated with this piece.
  AbstractBoard& getBoard() const;

  /// Returns the name of the piece (e.g. "Rook").
  virtual std::string name() const = 0;

  /**
   Prints the colour (owner) and the piece name as "Colour's PieceName".
   It requires name() to be overriden to return a piece name.
  */
  friend std::ostream& operator<<(std::ostream& out, Piece const& piece);

  virtual ~Piece() = default;

  Piece& operator=(Piece const&) = delete;
  Piece(Piece const&) = delete;
  Piece(Piece&&) = delete;
  Piece& operator=(Piece&&) = delete;

private:
  /// Checks if a piece can move according to its specific patterns.
  virtual bool isNormalMoveSpecific(Coordinates const& source,
                               Coordinates const& destination) const = 0;

  Colour m_colour;
  bool m_moved = false;
  std::reference_wrapper<AbstractBoard> m_board;
};

/// Represents a piece that can be used to promote a pawn.
class PromotionPiece: public Piece {
public:
  PromotionPiece(Colour colour, AbstractBoard& board): Piece(colour, board) {};
};

}

#endif // PIECE_H
