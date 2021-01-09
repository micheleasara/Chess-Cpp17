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
  /// Defines the colour of a piece.
  enum class Colour:int { White = 0, Black };

  /// Creates a chess piece of the given colour and associated to a board.
  Piece(Colour colour, Board& board);

  /**
  Verifies if the piece would be able to move from the source to the
  destination according to its internal logic. Checks or castling are
  considered board-level moves, and are not taken into account.
  
  Performs integrity checks shared among all pieces. For example:
  coordinates must be within limits, a piece cannot move to a square
  taken by an ally and source and destination cannot match.

  Returns the derived class' isMovePlausibleSpecific() to check the logic
  of each piece.
  */
  bool isMovePlausible(Coordinates const& source,
                       Coordinates const& destination) const;

  //! @copydoc Board::move(Pawn&,Coordinates&,Coordinates&)
  virtual MoveResult move(Coordinates const& source, 
                          Coordinates const& destination) = 0;

  /// Flags the piece to mean it has moved before.
  void setMovedStatus(bool moved);
  /// Returns whether the piece has moved before.
  bool getMovedStatus() const noexcept;

  /// Returns the colour of the piece.
  Colour getColour() const;

  /// Returns the name of the piece (eg "Rook").
  virtual std::string getName() const = 0;

  /**
  Prints the colour (owner) and the piece name as "Colour's PieceName".
  It requires getName() to be overriden to return a piece name.
  */
  friend std::ostream& operator<<(std::ostream& out, const Piece& piece);

  virtual ~Piece(){};
  Piece& operator=(Piece const&) = default;
  Piece(Piece const&) = default;
  Piece(Piece&&) = default;
  Piece& operator=(Piece&&) = default;

private:
  // Checks if a piece can move according to its specific patterns
  virtual bool isMovePlausibleSpecific(Coordinates const& source,
                               Coordinates const& destination) const = 0;

  Colour colour;
  bool moved = false;

protected:
  Board& board;
};

/// Represents a piece that can be used for a promotion.
class PromotionPiece: public Piece {
public:
  PromotionPiece(Colour colour, Board& board): Piece(colour, board) {};
};
}

#endif // PIECE_H
