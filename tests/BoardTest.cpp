#include "pch.h"
#include "Board.hpp"
#include "MoveResult.hpp"

using Chess::InvalidMove;
using Chess::Board;
using Chess::MoveResult;
using Chess::PromotionOption;
using Chess::Coordinates;
using Chess::CastlingType;

class BoardTest : public ::testing::Test {
protected:
  Board board;

  void moveAndTestThrow(std::string const& src, std::string const& dest,
                                         InvalidMove::ErrorCode err) {
    EXPECT_THROW({
     try {
       board.move(src, dest);
     } catch (InvalidMove const& e) {
      EXPECT_EQ(e.errorCode(), err);
      throw e;
     }}, Chess::InvalidMove);
  }

  void movePawnsForPromotion() {
    board.move("B2", "B4"); board.move("H7", "H5");
    board.move("B4", "B5"); board.move("H5", "H4");
    board.move("B5", "B6"); board.move("H4", "H3");
    board.move("B6", "C7"); board.move("H3", "G2");
  }

  void doThreeFoldRepetition() {
    board.move("D2", "D3"); board.move("D7", "D5");

    board.move("D1", "D2"); board.move("D8", "D7");
    board.move("D2", "D1"); board.move("D7", "D8");
    board.move("D1", "D2"); board.move("D8", "D7");
    EXPECT_FALSE(board.drawCanBeClaimed());
    board.move("D2", "D1"); board.move("D7", "D8");
  }
};

TEST_F(BoardTest, throwsIfCoordinatesAreInvalid) {
  moveAndTestThrow("A9", "A2", InvalidMove::ErrorCode::INVALID_COORDINATES);
  moveAndTestThrow("AA", "A2", InvalidMove::ErrorCode::INVALID_COORDINATES);
  moveAndTestThrow("", "A2", InvalidMove::ErrorCode::INVALID_COORDINATES);
  moveAndTestThrow("A-1", "A2", InvalidMove::ErrorCode::INVALID_COORDINATES);
  moveAndTestThrow("A2", "A9", InvalidMove::ErrorCode::INVALID_COORDINATES);
  moveAndTestThrow("A2", "AA", InvalidMove::ErrorCode::INVALID_COORDINATES);
  moveAndTestThrow("A2", "", InvalidMove::ErrorCode::INVALID_COORDINATES);
  moveAndTestThrow("A2", "A-1", InvalidMove::ErrorCode::INVALID_COORDINATES);
}
TEST_F(BoardTest, throwsIfSourcePieceNotFound) {
  moveAndTestThrow("A3", "A4", InvalidMove::ErrorCode::NO_SOURCE_PIECE);
}

TEST_F(BoardTest, throwsIfWrongTurn) {
  board.move("A2", "A3");
  moveAndTestThrow("A3", "A4", InvalidMove::ErrorCode::WRONG_TURN);
  board.move("A7", "A6");
  moveAndTestThrow("A6", "A5", InvalidMove::ErrorCode::WRONG_TURN);

}
TEST_F(BoardTest, throwsIfPieceLogicPreventsMove) {
  moveAndTestThrow("A2", "B3", InvalidMove::ErrorCode::PIECE_LOGIC_ERROR);
  moveAndTestThrow("A1", "B3", InvalidMove::ErrorCode::PIECE_LOGIC_ERROR);
  moveAndTestThrow("B1", "B3", InvalidMove::ErrorCode::PIECE_LOGIC_ERROR);
  moveAndTestThrow("C1", "B3", InvalidMove::ErrorCode::PIECE_LOGIC_ERROR);
  moveAndTestThrow("D1", "B3", InvalidMove::ErrorCode::PIECE_LOGIC_ERROR);
  moveAndTestThrow("E1", "B3", InvalidMove::ErrorCode::PIECE_LOGIC_ERROR);
}

TEST_F(BoardTest, throwsIfMoveWouldCauseOwnCheck) {
  board.move("B2", "B3");
  board.move("C7", "C5");
  board.move("G2", "G3");
  board.move("D8", "A5");
  moveAndTestThrow("D2", "D3", InvalidMove::ErrorCode::CHECK_ERROR);
}

TEST_F(BoardTest, throwsIfAttemptedMoveWhilePromotionIsPending) {
  board.move("B2", "B4");
  board.move("C7", "C6");
  board.move("B4", "B5");
  board.move("D8", "B6");
  board.move("B5", "C6");
  board.move("B6", "D8");
  board.move("C6", "B7");
  board.move("H7", "H6");
  board.move("B7", "C8");

  moveAndTestThrow("H6", "H5", InvalidMove::ErrorCode::PENDING_PROMOTION);
  moveAndTestThrow("C8", "D8", InvalidMove::ErrorCode::PENDING_PROMOTION);
}

TEST_F(BoardTest, normalMovesLeaveTheGameInANormalState) {
  auto state = board.move("B2", "B3").gameState();
  EXPECT_EQ(state, MoveResult::GameState::NORMAL);
  state = board.move("G8", "H6").gameState();
  EXPECT_EQ(state, MoveResult::GameState::NORMAL);
  state = board.move("C1", "B2").gameState();
  EXPECT_EQ(state, MoveResult::GameState::NORMAL);
}

TEST_F(BoardTest, blackCanBePutInCheck) {
  board.move("C2", "C3");
  board.move("D7", "D5");
  auto state = board.move("D1", "A4").gameState();
  EXPECT_EQ(state, MoveResult::GameState::OPPONENT_IN_CHECK);
}

TEST_F(BoardTest, whiteCanBePutInCheck) {
  board.move("D2", "D3");
  board.move("C7", "C5");
  board.move("H2", "H3");
  auto state = board.move("D8", "A5").gameState();
  EXPECT_EQ(state, MoveResult::GameState::OPPONENT_IN_CHECK);
}

TEST_F(BoardTest, blackCanBePutInCheckmate) {
  board.move("E2", "E4");
  board.move("F7", "F6");
  board.move("D2", "D3");
  board.move("G7", "G5");
  auto state = board.move("D1", "H5").gameState();
  EXPECT_EQ(state, MoveResult::GameState::OPPONENT_IN_CHECKMATE);
  EXPECT_TRUE(board.isGameOver());
}

TEST_F(BoardTest, whiteCanBePutInCheckmate) {
  board.move("F2", "F3");
  board.move("E7", "E5");
  board.move("G2", "G4");
  auto state = board.move("D8", "H4").gameState();
  EXPECT_EQ(state, MoveResult::GameState::OPPONENT_IN_CHECKMATE);
  EXPECT_TRUE(board.isGameOver());
}

TEST_F(BoardTest, throwsIfAttemptMoveAfterGameOverWithoutReset) {
  board.move("F2", "F3"); board.move("E7", "E5");
  board.move("G2", "G4");
  auto state = board.move("D8", "H4").gameState();
  EXPECT_EQ(state, MoveResult::GameState::OPPONENT_IN_CHECKMATE);
  moveAndTestThrow("D2", "D3", InvalidMove::ErrorCode::GAME_OVER);
}

TEST_F(BoardTest, noThrowIfMoveAfterGameOverWithReset) {
  board.move("F2", "F3"); board.move("E7", "E5");
  board.move("G2", "G4");
  auto state = board.move("D8", "H4").gameState();
  EXPECT_EQ(state, MoveResult::GameState::OPPONENT_IN_CHECKMATE);

  board.reset();
  state = board.move("A2", "A3").gameState();
  EXPECT_EQ(state, MoveResult::GameState::NORMAL);
}

TEST_F(BoardTest, gameCanStalemate) {
  board.move("E2", "E3"); board.move("A7", "A5");
  board.move("D1", "H5"); board.move("A8", "A6");
  board.move("H5", "A5"); board.move("H7", "H5");
  board.move("A5", "C7"); board.move("A6", "H6");
  board.move("H2", "H4"); board.move("F7", "F6");
  board.move("C7", "D7"); board.move("E8", "F7");
  board.move("D7", "B7"); board.move("D8", "D3");
  board.move("B7", "B8"); board.move("D3", "H7");
  board.move("B8", "C8"); board.move("F7", "G6");

  auto state = board.move("C8", "E6").gameState();
  EXPECT_EQ(state, MoveResult::GameState::STALEMATE);
  EXPECT_TRUE(board.isGameOver());
}

TEST_F(BoardTest, canPromoteWithoutEatingInTheLastMove) {
  movePawnsForPromotion();
  board.move("E2", "E3"); board.move("B7", "B6");
  board.move("G1", "H3"); board.move("C8", "B7");

  auto result = board.move("C7", "C8");
  EXPECT_TRUE(board.isPromotionPending());
  EXPECT_EQ(result.capturedPieceName(), std::nullopt);
  EXPECT_EQ(result.gameState(), MoveResult::GameState::AWAITING_PROMOTION);
  board.promote(PromotionOption::Queen);

  result = board.move("G2", "G1");
  EXPECT_TRUE(board.isPromotionPending());
  EXPECT_EQ(result.capturedPieceName(), std::nullopt);
  EXPECT_EQ(result.gameState(), MoveResult::GameState::AWAITING_PROMOTION);
}

TEST_F(BoardTest, canPromoteByEatingInTheLastMove) {
  movePawnsForPromotion();
  auto result = board.move("C7", "B8");
  EXPECT_TRUE(board.isPromotionPending());
  EXPECT_NE(result.capturedPieceName(), std::nullopt);
  EXPECT_EQ(result.gameState(), MoveResult::GameState::AWAITING_PROMOTION);
  board.promote(PromotionOption::Queen);

  result = board.move("G2", "F1");
  EXPECT_TRUE(board.isPromotionPending());
  EXPECT_NE(result.capturedPieceName(), std::nullopt);
  EXPECT_EQ(result.gameState(), MoveResult::GameState::AWAITING_PROMOTION);
}

TEST_F(BoardTest, afterPromotionIsDoneThereIsNoPromotionPending) {
  movePawnsForPromotion();
  board.move("C7", "B8");
  board.promote(PromotionOption::Queen);
  EXPECT_FALSE(board.isPromotionPending());
}

TEST_F(BoardTest, whiteCanPromoteAndPutBlackInCheck) {
  movePawnsForPromotion();
  board.move("C7", "D8");
  auto result = board.promote(PromotionOption::Queen);
  EXPECT_EQ(result.gameState(), MoveResult::GameState::OPPONENT_IN_CHECK);
}

TEST_F(BoardTest, blackCanPromoteAndPutWhiteInCheck) {
  movePawnsForPromotion();
  board.move("A2", "A3");

  board.move("G2", "F1");
  auto result = board.promote(PromotionOption::Rook);
  EXPECT_EQ(result.gameState(), MoveResult::GameState::OPPONENT_IN_CHECK);
}

TEST_F(BoardTest, threeFoldRepetitionAllowsToClaimDraw) {
  doThreeFoldRepetition();
  EXPECT_TRUE(board.drawCanBeClaimed());
  board.move("D1", "D2"); board.move("D8", "D7");
  EXPECT_TRUE(board.drawCanBeClaimed());
}

TEST_F(BoardTest, claimingDrawWhenAppropriateEndsTheGame) {
  doThreeFoldRepetition();
  EXPECT_FALSE(board.isGameOver());
  board.claimDraw();
  EXPECT_TRUE(board.isGameOver());
}

TEST_F(BoardTest, claimingDrawWhenNotAppropriateDoesNothing) {
  EXPECT_FALSE(board.drawCanBeClaimed());
  board.claimDraw();
  EXPECT_FALSE(board.isGameOver());
}

TEST_F(BoardTest, fiveFoldRepetitionForcesDraw) {
  doThreeFoldRepetition();
  board.move("D1", "D2"); board.move("D8", "D7");
  board.move("D2", "D1"); board.move("D7", "D8");
  board.move("D1", "D2"); board.move("D8", "D7");
  board.move("D2", "D1");
  EXPECT_FALSE(board.isGameOver());
  auto state = board.move("D7", "D8").gameState();
  EXPECT_EQ(state, MoveResult::GameState::FIVEFOLD_REPETITION_DRAW);
  EXPECT_TRUE(board.isGameOver());
}

TEST_F(BoardTest, canCastleKingSide) {
  board.move("G1", "F3"); board.move("G8", "F6");
  board.move("G2", "G3"); board.move("G7", "G6");
  board.move("F1", "G2"); board.move("F8", "G7");

  auto& whiteK = board.getPieceAtCoordinates(Coordinates(4, 0))->get();
  auto& whiteR = board.getPieceAtCoordinates(Coordinates(7, 0))->get();
  auto result = board.move("E1", "G1");
  ASSERT_TRUE(result.castlingType().has_value());
  auto castlingType = *result.castlingType();
  EXPECT_EQ(result.gameState(), MoveResult::GameState::NORMAL);
  EXPECT_EQ(castlingType, CastlingType::KingSide);
  auto& expectedWhiteK = board.getPieceAtCoordinates(Coordinates(6, 0))->get();
  auto& expectedWhiteR = board.getPieceAtCoordinates(Coordinates(5, 0))->get();
  EXPECT_EQ(&whiteK, &expectedWhiteK);
  EXPECT_EQ(&whiteR, &expectedWhiteR);

  auto& blackK = board.getPieceAtCoordinates(Coordinates(4, 7))->get();
  auto& blackR = board.getPieceAtCoordinates(Coordinates(7, 7))->get();
  result = board.move("E8", "G8");
  ASSERT_TRUE(result.castlingType().has_value());
  EXPECT_EQ(result.gameState(), MoveResult::GameState::NORMAL);
  castlingType = *result.castlingType();
  EXPECT_EQ(castlingType, CastlingType::KingSide);
  auto& expectedBlackK = board.getPieceAtCoordinates(Coordinates(6, 7))->get();
  auto& expectedBlackR = board.getPieceAtCoordinates(Coordinates(5, 7))->get();
  EXPECT_EQ(&blackK, &expectedBlackK);
  EXPECT_EQ(&blackR, &expectedBlackR);
}

TEST_F(BoardTest, canCastleQueenSide) {
  board.move("B2", "B3"); board.move("B7", "B6");
  board.move("C2", "C3"); board.move("C7", "C6");
  board.move("C1", "B2"); board.move("C8", "B7");
  board.move("B1", "A3"); board.move("B8", "A6");
  board.move("D1", "C2"); board.move("D8", "C7");

  auto& whiteK = board.getPieceAtCoordinates(Coordinates(4, 0))->get();
  auto& whiteR = board.getPieceAtCoordinates(Coordinates(0, 0))->get();
  auto result = board.move("E1", "C1");
  EXPECT_EQ(result.gameState(), MoveResult::GameState::NORMAL);
  ASSERT_TRUE(result.castlingType().has_value());
  auto castlingType = *result.castlingType();
  EXPECT_EQ(castlingType, CastlingType::QueenSide);
  auto& expectedWhiteK = board.getPieceAtCoordinates(Coordinates(2, 0))->get();
  auto& expectedWhiteR = board.getPieceAtCoordinates(Coordinates(3, 0))->get();
  EXPECT_EQ(&whiteK, &expectedWhiteK);
  EXPECT_EQ(&whiteR, &expectedWhiteR);

  auto& blackK = board.getPieceAtCoordinates(Coordinates(4, 7))->get();
  auto& blackR = board.getPieceAtCoordinates(Coordinates(0, 7))->get();
  result = board.move("E8", "C8");
  ASSERT_TRUE(result.castlingType().has_value());
  EXPECT_EQ(result.gameState(), MoveResult::GameState::NORMAL);
  castlingType = *result.castlingType();
  EXPECT_EQ(castlingType, CastlingType::QueenSide);
  auto& expectedBlackK = board.getPieceAtCoordinates(Coordinates(2, 7))->get();
  auto& expectedBlackR = board.getPieceAtCoordinates(Coordinates(3, 7))->get();
  EXPECT_EQ(&blackK, &expectedBlackK);
  EXPECT_EQ(&blackR, &expectedBlackR);
}

TEST_F(BoardTest, cannotCastleIfKingHasMoved) {
  board.move("G1", "F3"); board.move("G8", "F6");
  board.move("G2", "G3"); board.move("G7", "G6");
  board.move("F1", "G2"); board.move("F8", "G7");
  board.move("E1", "F1"); board.move("E8", "F8");
  board.move("F1", "E1"); board.move("F8", "E8");

  auto& whiteK = board.getPieceAtCoordinates(Coordinates(4, 0))->get();
  moveAndTestThrow("E1", "G1", InvalidMove::ErrorCode::PIECE_LOGIC_ERROR);
}

TEST_F(BoardTest, cannotCastleIfRookHasMoved) {
  board.move("G1", "F3"); board.move("G8", "F6");
  board.move("G2", "G3"); board.move("G7", "G6");
  board.move("F1", "G2"); board.move("F8", "G7");
  board.move("H1", "G1"); board.move("H8", "G8");
  board.move("G1", "H1"); board.move("G8", "H8");

  auto& whiteK = board.getPieceAtCoordinates(Coordinates(4, 0))->get();
  moveAndTestThrow("E1", "G1", InvalidMove::ErrorCode::PIECE_LOGIC_ERROR);
}

TEST_F(BoardTest, canClaimDrawAccordingToFiftyMovesRule) {
  // Andrzej Filipowicz vs Petar Smederevac (1966)
  // there are no captures in this game
  board.move("E2", "E4"); board.move("E7", "E6");
  board.move("D2", "D3"); board.move("G8", "E7");
  board.move("G2", "G3"); board.move("C7", "C5");
  board.move("F1", "G2"); board.move("B8", "C6");
  board.move("C1", "E3"); board.move("B7", "B6");
  board.move("G1", "E2"); board.move("D7", "D5");
  board.move("E1", "G1"); board.move("D5", "D4");
  board.move("E3", "C1"); board.move("G7", "G6");
  board.move("B1", "D2"); board.move("F8", "G7");
  board.move("F2", "F4"); board.move("F7", "F5");
  board.move("A2", "A3"); board.move("E8", "G8");
  board.move("E4", "E5"); board.move("A7", "A5");
  board.move("A3", "A4"); board.move("C8", "A6");
  board.move("B2", "B3"); board.move("A8", "B8");
  board.move("D2", "C4"); board.move("D8", "C7");
  board.move("G1", "H1"); board.move("E7", "D5");
  board.move("C1", "D2"); board.move("F8", "D8");
  board.move("E2", "G1"); board.move("G7", "F8");
  board.move("G1", "F3"); board.move("F8", "E7");
  board.move("H2", "H4"); // last pawn movement
  board.move("H7", "H5");
  board.move("D1", "E2"); board.move("C6", "B4");
  board.move("F1", "C1"); board.move("A6", "B7");
  board.move("H1", "H2"); board.move("B7", "C6");
  board.move("C4", "A3"); board.move("B8", "A8");
  board.move("E2", "E1"); board.move("D8", "B8");
  board.move("E1", "G1"); board.move("C7", "B7");
  board.move("G1", "F1"); board.move("G8", "G7");
  board.move("F1", "H1"); board.move("B7", "D7");
  board.move("F3", "E1"); board.move("A8", "A7");
  board.move("E1", "F3"); board.move("B8", "A8");
  board.move("F3", "E1"); board.move("E7", "D8");
  board.move("E1", "F3"); board.move("A8", "B8");
  board.move("F3", "E1"); board.move("D8", "C7");
  board.move("E1", "F3"); board.move("B8", "H8");
  board.move("F3", "G5"); board.move("C7", "D8");
  board.move("G5", "F3"); board.move("D8", "E7");
  board.move("H1", "G1"); board.move("C6", "B7");
  board.move("A3", "B5"); board.move("A7", "A8");
  board.move("B5", "A3"); board.move("B7", "A6");
  board.move("G1", "F1"); board.move("A8", "B8");
  board.move("A3", "C4"); board.move("E7", "D8");
  board.move("F1", "D1"); board.move("D5", "E7");
  board.move("C4", "D6"); board.move("D8", "C7");
  board.move("D1", "E2"); board.move("E7", "G8");
  board.move("F3", "G5"); board.move("G8", "H6");
  board.move("G2", "F3"); board.move("C7", "D8");
  board.move("G5", "H3"); board.move("H6", "G4");
  board.move("H2", "G1"); board.move("D8", "E7");
  board.move("D6", "C4"); board.move("B4", "D5");
  board.move("H3", "F2"); board.move("A6", "B7");
  board.move("F2", "H3"); board.move("B7", "C6");
  board.move("E2", "G2"); board.move("H8", "C8");
  board.move("C1", "E1"); board.move("C8", "C7");
  board.move("E1", "E2"); board.move("C7", "A7");
  board.move("E2", "E1"); board.move("A7", "A6");
  board.move("E1", "E2"); board.move("B8", "A8");
  board.move("E2", "E1"); board.move("A8", "A7");
  board.move("C4", "A3"); board.move("A7", "A8");
  board.move("A3", "C4"); board.move("G4", "H6");
  board.move("C4", "A3"); board.move("H6", "F7");
  board.move("H3", "F2"); board.move("A8", "D8");
  board.move("A3", "C4"); board.move("D8", "B8");
  board.move("F2", "H3"); board.move("E7", "D8");
  board.move("C4", "A3"); board.move("A6", "A7");
  board.move("G2", "H1"); board.move("D8", "C7");
  board.move("H1", "G2"); board.move("B8", "D8");
  board.move("G2", "H1"); board.move("F7", "H6");
  board.move("H3", "G5"); board.move("H6", "F7");
  board.move("G5", "H3"); board.move("D7", "E8");
  board.move("G1", "H2");
  EXPECT_FALSE(board.drawCanBeClaimed());
  board.move("D8", "D7");
  EXPECT_TRUE(board.drawCanBeClaimed());
}

TEST_F(BoardTest, canDoAlekhineVsVasic1931) {
  board.move("E2", "E4"); board.move("E7", "E6");
  board.move("D2", "D4"); board.move("D7", "D5");
  board.move("B1", "C3"); board.move("F8", "B4");
  board.move("F1", "D3");
  auto state = board.move("B4", "C3").gameState(); // white in check
  EXPECT_EQ(state, MoveResult::GameState::OPPONENT_IN_CHECK);

  board.move("B2", "C3"); board.move("H7", "H6");
  board.move("C1", "A3"); board.move("B8", "D7");
  board.move("D1", "E2"); board.move("D5", "E4");
  board.move("D3", "E4"); board.move("G8", "F6");
  board.move("E4", "D3"); board.move("B7", "B6");
  state = board.move("E2", "E6").gameState(); // black in check
  EXPECT_EQ(state, MoveResult::GameState::OPPONENT_IN_CHECK);
  board.move("F7", "E6");

  state = board.move("D3", "G6").gameState(); // white wins
  EXPECT_EQ(state, MoveResult::GameState::OPPONENT_IN_CHECKMATE);
  EXPECT_TRUE(board.isGameOver());
}