#ifndef CHESS_MOVE_RESULT
#define CHESS_MOVE_RESULT

#include <optional>
#include <string>
#include "Utils.hpp"

namespace Chess {

/// Represents the outcome of a valid move.
class MoveResult {
public:
  /// Represents the state of the game after the move.
  enum class GameState {
    /// A normal state without checks, checkmates or other similar events.
    NORMAL,
    /// The opponent is in check.
    OPPONENT_IN_CHECK,
    /// The opponent has been checkmated.
    OPPONENT_IN_CHECKMATE,
    /// The game is in stalemate.
    STALEMATE,
    /// The game is a draw due to the five-fold repetition rule.
    FIVEFOLD_REPETITION_DRAW,
    /// The game is a draw due to the seventy-five moves rule.
    SEVENTYFIVE_MOVES_DRAW,
    /// The game is a draw as there is no sufficient material for a checkmate.
    INSUFFICIENT_MATERIAL_DRAW,
    /// Promotion is currently pending.
    AWAITING_PROMOTION
  };

  /// Constructs a move result with the given state.
  MoveResult(GameState state);
  /// Constructs a move result with a state and the name of a captured piece.
  MoveResult(GameState state, std::string capturedPieceName);
  /// Constructs a move result with a state and a castling type.
  MoveResult(GameState state, CastlingType castlingType);

  /**
  Returns the name of the captured piece, or an empty optional if no piece
  was captured.
  */
  std::optional<std::string> capturedPieceName() const;

  /// Returns the state of the game after the move.
  GameState gameState() const;
  
  /// Returns the castling type, or an empty optional if no castling occurred.
  std::optional<CastlingType> castlingType() const;

private:
  GameState m_gameState;
  std::optional<std::string> m_capturedPieceName;
  std::optional<CastlingType> m_castlingType;
};

}
#endif // CHESS_MOVE_RESULT
