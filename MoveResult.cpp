#include "MoveResult.hpp"

namespace Chess {

MoveResult::MoveResult(GameState state): m_gameState(state) {}

MoveResult::MoveResult(GameState gameState, std::string capturedPieceName):
   m_gameState(gameState), m_capturedPieceName(std::move(capturedPieceName)) {}

MoveResult::MoveResult(GameState state, CastlingType castlingType) :
  m_gameState(state), m_castlingType(castlingType) {}

std::optional<std::string> MoveResult::capturedPieceName() const {
  return m_capturedPieceName;
}

MoveResult::GameState MoveResult::gameState() const {
  return m_gameState;
}

std::optional<CastlingType> MoveResult::castlingType() const {
    return m_castlingType;
}

}