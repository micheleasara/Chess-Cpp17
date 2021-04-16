#ifndef CHESS_EXCEPTIONS
#define CHESS_EXCEPTIONS

#include <stdexcept>
#include <string>

namespace Chess {

/// An exception thrown in case of an invalid move being attempted.
class InvalidMove : public std::logic_error {
public:
  /// Error codes specifying a type of invalid move.
  enum class ErrorCode {
    /// The coordinates given were not valid.
    INVALID_COORDINATES,
    /// There was an attempt to move a piece when it was not the player's turn.
    WRONG_TURN,
    /// No piece at the given source coordinates.
    NO_SOURCE_PIECE,
    /// The selected piece cannot move in that way.
    PIECE_LOGIC_ERROR,
    /// The move causes a self-check, or does not resolve an existing check.
    CHECK_ERROR,
    /// Promotion is still pending.
    PENDING_PROMOTION,
    /// The game is over.
    GAME_OVER
  };

  /// Constructs an exception with the explanation and code provided.
  InvalidMove(std::string const& what, ErrorCode code);

  /// Returns the associated error code.
  ErrorCode errorCode() const;

private:
  ErrorCode m_errorCode;
};

}
#endif // CHESS_EXCEPTIONS
