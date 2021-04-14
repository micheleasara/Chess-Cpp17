#ifndef PIECE_H
#define PIECE_H

#include <ostream>
#include "Utils.hpp"
#include "MoveResult.hpp"

namespace Chess {
class Board;

/// Represents a generic piece.
class Piece {
public:
  /// Creates a chess piece of the given colour and associated with a board.
  Piece(Colour colour, Board& board);

  /**
   Returns whether the piece would be able to move from the source to the
   destination according to its internal logic. Checks or castling are
   considered board-level moves, and are not taken into account.
  
   Returns false if certain conditions common to all pieces are not satisfied.
   For example:
   coordinates must be within the board limits, a piece cannot move to a square
   taken by an ally, and source and destination cannot match.

   Returns the derived class' isMovePlausibleSpecific() to check the logic of
   each piece.
  */
  bool isMovePlausible(Coordinates const& source,
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
  void setBoard(Board& board) noexcept;

  /// Returns the board currently associated with this piece.
  Board& getBoard() const;

  /// Returns the name of the piece (e.g. "Rook").
  virtual std::string getName() const = 0;

  /**
   Prints the colour (owner) and the piece name as "Colour's PieceName".
   It requires getName() to be overriden to return a piece name.
  */
  friend std::ostream& operator<<(std::ostream& out, Piece const& piece);

  virtual ~Piece() = default;

  Piece& operator=(Piece const&) = delete;
  Piece(Piece const&) = delete;
  Piece(Piece&&) = delete;
  Piece& operator=(Piece&&) = delete;

private:
  /// Checks if a piece can move according to its specific patterns.
  virtual bool isMovePlausibleSpecific(Coordinates const& source,
                               Coordinates const& destination) const = 0;

  Colour m_colour;
  bool m_moved = false;
  std::reference_wrapper<Board> m_board;
};

/// Represents a piece that can be used to promote a pawn.
class PromotionPiece: public Piece {
public:
  PromotionPiece(Colour colour, Board& board): Piece(colour, board) {};
};
}

#endif // PIECE_H
