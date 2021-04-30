#include "pch.h"
#include "Board.hpp"
#include "BoardHasherMock.hpp"
#include "MoveResult.hpp"
#include "Zobrist.hpp"

using Chess::InvalidMove;
using Chess::Board;
using Chess::MoveResult;
using Chess::PromotionOption;
using Chess::Coordinates;
using Chess::CastlingType;
using ::testing::AtLeast;
using ::testing::NiceMock;

class BoardTest : public ::testing::Test {
protected:
  Board board;

  void moveAndTestThrow(std::string_view src, std::string_view dest,
                                         InvalidMove::ErrorCode err) {
    EXPECT_THROW({
     try {
       board.move(src, dest);
     } catch (InvalidMove const& e) {
      EXPECT_EQ(e.errorCode(), err);
      throw e;
     }}, Chess::InvalidMove);
  }

  decltype(auto) buildNiceBoardHasherMock() {
    return std::make_unique<NiceMock<BoardHasherMock>>();
  }

  void movePawnsForPromotion() {
    movePawnsForPromotion(board);
  }

  void movePawnsForPromotion(Board& board) {
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

  void testAlekhineVsVasic1931() {
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

TEST_F(BoardTest, throwsIfTheBoardIsConstructedWithANullHasher) {
  EXPECT_THROW(board = Board(nullptr), std::invalid_argument);
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

TEST_F(BoardTest, canPromoteWithoutCapturingInTheLastMove) {
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

TEST_F(BoardTest, canPromoteByCapturingInTheLastMove) {
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
  EXPECT_EQ(result->gameState(), MoveResult::GameState::OPPONENT_IN_CHECK);
}

TEST_F(BoardTest, blackCanPromoteAndPutWhiteInCheck) {
  movePawnsForPromotion();
  board.move("A2", "A3");

  board.move("G2", "F1");
  auto result = board.promote(PromotionOption::Rook);
  EXPECT_EQ(result->gameState(), MoveResult::GameState::OPPONENT_IN_CHECK);
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

  auto whiteK = board.at(Coordinates(4, 0));
  auto whiteR = board.at(Coordinates(7, 0));
  auto result = board.move("E1", "G1");
  ASSERT_TRUE(result.castlingType().has_value());
  auto castlingType = *result.castlingType();
  EXPECT_EQ(result.gameState(), MoveResult::GameState::NORMAL);
  EXPECT_EQ(castlingType, CastlingType::KingSide);
  EXPECT_EQ(whiteK, board.at(Coordinates(6, 0)));
  EXPECT_EQ(whiteR, board.at(Coordinates(5, 0)));

  auto blackK = board.at(Coordinates(4, 7));
  auto blackR = board.at(Coordinates(7, 7));
  result = board.move("E8", "G8");
  ASSERT_TRUE(result.castlingType().has_value());
  EXPECT_EQ(result.gameState(), MoveResult::GameState::NORMAL);
  castlingType = *result.castlingType();
  EXPECT_EQ(castlingType, CastlingType::KingSide);
  EXPECT_EQ(blackK, board.at(Coordinates(6, 7)));
  EXPECT_EQ(blackR, board.at(Coordinates(5, 7)));
}

TEST_F(BoardTest, canCastleQueenSide) {
  board.move("B2", "B3"); board.move("B7", "B6");
  board.move("C2", "C3"); board.move("C7", "C6");
  board.move("C1", "B2"); board.move("C8", "B7");
  board.move("B1", "A3"); board.move("B8", "A6");
  board.move("D1", "C2"); board.move("D8", "C7");

  auto whiteK = board.at(Coordinates(4, 0));
  auto whiteR = board.at(Coordinates(0, 0));
  auto result = board.move("E1", "C1");
  EXPECT_EQ(result.gameState(), MoveResult::GameState::NORMAL);
  ASSERT_TRUE(result.castlingType().has_value());
  auto castlingType = *result.castlingType();
  EXPECT_EQ(castlingType, CastlingType::QueenSide);
  EXPECT_EQ(whiteK, board.at(Coordinates(2, 0)));
  EXPECT_EQ(whiteR, board.at(Coordinates(3, 0)));

  auto blackK = board.at(Coordinates(4, 7));
  auto blackR = board.at(Coordinates(0, 7));
  result = board.move("E8", "C8");
  ASSERT_TRUE(result.castlingType().has_value());
  EXPECT_EQ(result.gameState(), MoveResult::GameState::NORMAL);
  castlingType = *result.castlingType();
  EXPECT_EQ(castlingType, CastlingType::QueenSide);
  EXPECT_EQ(blackK, board.at(Coordinates(2, 7)));
  EXPECT_EQ(blackR, board.at(Coordinates(3, 7)));
}

TEST_F(BoardTest, cannotCastleIfKingHasMoved) {
  board.move("G1", "F3"); board.move("G8", "F6");
  board.move("G2", "G3"); board.move("G7", "G6");
  board.move("F1", "G2"); board.move("F8", "G7");
  board.move("E1", "F1"); board.move("E8", "F8");
  board.move("F1", "E1"); board.move("F8", "E8");

  moveAndTestThrow("E1", "G1", InvalidMove::ErrorCode::PIECE_LOGIC_ERROR);
}

TEST_F(BoardTest, cannotCastleIfRookHasMoved) {
  board.move("G1", "F3"); board.move("G8", "F6");
  board.move("G2", "G3"); board.move("G7", "G6");
  board.move("F1", "G2"); board.move("F8", "G7");
  board.move("H1", "G1"); board.move("H8", "G8");
  board.move("G1", "H1"); board.move("G8", "H8");

  moveAndTestThrow("E1", "G1", InvalidMove::ErrorCode::PIECE_LOGIC_ERROR);
}

TEST_F(BoardTest, undoingWithNoRecordedMovesDoesNothing) {
  EXPECT_NO_THROW(board.undoLastMove());
}

TEST_F(BoardTest, canUndoNonCapturingMove) {
  auto piece = board.at(Coordinates(6, 0));
  auto hasMoved = piece->getMovedStatus();
  board.move("G1", "F3");

  board.undoLastMove();
  auto expectedPiece = board.at(Coordinates(6, 0));
  EXPECT_EQ(expectedPiece, piece);
  EXPECT_EQ(hasMoved, piece->getMovedStatus());
  EXPECT_TRUE(board.at(Coordinates(5, 2)) == nullptr);
}

TEST_F(BoardTest, canUndoCapturingMove) {
  board.move("A2", "A4"); board.move("B7", "B5");
  auto capturingPiece = board.at(Coordinates(0, 3));
  auto capturedPiece = board.at(Coordinates(1, 4));
  board.move("A4", "B5");

  board.undoLastMove();
  auto expectedCaptured = board.at(Coordinates(1, 4));
  ASSERT_FALSE(expectedCaptured == nullptr);
  EXPECT_EQ(expectedCaptured, capturedPiece);

  auto expectedCapturing = board.at(Coordinates(0, 3));
  ASSERT_FALSE(expectedCapturing == nullptr);
  EXPECT_EQ(expectedCapturing, capturingPiece);
}

TEST_F(BoardTest, canUndoEnPassant) {
  board.move("E2", "E4"); board.move("H7", "H5");
  board.move("E4", "E5"); board.move("D7", "D5");
  auto wPawn = board.at(Coordinates(4, 4));
  auto bPawn = board.at(Coordinates(3, 4));
  board.move("E5", "D6");

  board.undoLastMove();
  auto expectedWPawn = board.at(Coordinates(4, 4));
  auto expectedBPawn = board.at(Coordinates(3, 4));
  ASSERT_FALSE(expectedBPawn == nullptr);
  ASSERT_FALSE(expectedWPawn == nullptr);
  EXPECT_EQ(expectedBPawn, bPawn);
  EXPECT_EQ(expectedWPawn, wPawn);
  EXPECT_TRUE(board.at(Coordinates(3, 5)) == nullptr);
}

TEST_F(BoardTest, canUndoCastlingKingSide) {
  board.move("G1", "F3"); board.move("G8", "F6");
  board.move("G2", "G3"); board.move("G7", "G6");
  board.move("F1", "G2"); board.move("F8", "G7");
  board.move("E1", "G1");
  auto whiteK = board.at(Coordinates(6, 0));
  auto whiteR = board.at(Coordinates(5, 0));

  board.undoLastMove();
  auto expectedWhiteK = board.at(Coordinates(4, 0));
  auto expectedWhiteR = board.at(Coordinates(7, 0));
  EXPECT_EQ(whiteK, expectedWhiteK);
  EXPECT_EQ(whiteR, expectedWhiteR);
  EXPECT_FALSE(whiteK->getMovedStatus());
  EXPECT_FALSE(whiteR->getMovedStatus());
}

TEST_F(BoardTest, canUndoCastlingQueenSide) {
  board.move("B2", "B3"); board.move("B7", "B6");
  board.move("C2", "C3"); board.move("C7", "C6");
  board.move("C1", "B2"); board.move("C8", "B7");
  board.move("B1", "A3"); board.move("B8", "A6");
  board.move("D1", "C2"); board.move("D8", "C7");
  board.move("E1", "C1");
  auto whiteK = board.at(Coordinates(2, 0));
  auto whiteR = board.at(Coordinates(3, 0));

  board.undoLastMove();
  EXPECT_EQ(whiteK, board.at(Coordinates(4, 0)));
  EXPECT_EQ(whiteR, board.at(Coordinates(0, 0)));
  EXPECT_FALSE(whiteK->getMovedStatus());
  EXPECT_FALSE(whiteR->getMovedStatus());
}

TEST_F(BoardTest, canUndoPromotion) {
  movePawnsForPromotion();
  board.move("E2", "E3"); board.move("B7", "B6");
  board.move("G1", "H3"); board.move("C8", "B7");
  board.move("C7", "C8");
  auto pawn = board.at(Coordinates(2, 7));
  board.promote(PromotionOption::Queen);

  board.undoLastMove();
  EXPECT_FALSE(board.isPromotionPending());
  auto expectedPawn = board.at(Coordinates(2, 6));
  ASSERT_FALSE(expectedPawn == nullptr);
  EXPECT_EQ(expectedPawn, pawn);
  EXPECT_TRUE(board.at(Coordinates(2, 7)) == nullptr);
}

TEST_F(BoardTest, canUndoStalemate) {
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

  board.undoLastMove();
  state = board.move("A2", "A3").gameState();
  EXPECT_EQ(state, MoveResult::GameState::NORMAL);
  EXPECT_FALSE(board.isGameOver());
}

TEST_F(BoardTest, canUndoThreeFoldRepetition) {
  doThreeFoldRepetition();
  EXPECT_TRUE(board.drawCanBeClaimed());

  board.undoLastMove();
  EXPECT_FALSE(board.drawCanBeClaimed());
}

TEST_F(BoardTest, canUndoFiveFoldRepetition) {
  doThreeFoldRepetition();
  board.move("D1", "D2"); board.move("D8", "D7");
  board.move("D2", "D1"); board.move("D7", "D8");
  board.move("D1", "D2"); board.move("D8", "D7");
  board.move("D2", "D1");
  auto state = board.move("D7", "D8").gameState();
  EXPECT_EQ(state, MoveResult::GameState::FIVEFOLD_REPETITION_DRAW);
  EXPECT_TRUE(board.isGameOver());

  board.undoLastMove();
  EXPECT_FALSE(board.isGameOver());
  state = board.move("F7", "F6").gameState();
  EXPECT_EQ(state, MoveResult::GameState::NORMAL);
}

TEST_F(BoardTest, canUndoCheck) {
  board.move("E2", "E4"); board.move("E7", "E6");
  board.move("D2", "D4"); board.move("D7", "D5");
  board.move("B1", "C3"); board.move("F8", "B4");
  board.move("F1", "D3");
  auto state = board.move("B4", "C3").gameState();
  EXPECT_EQ(state, MoveResult::GameState::OPPONENT_IN_CHECK);

  board.undoLastMove();
  state = board.move("A7", "A6").gameState();
  EXPECT_EQ(state, MoveResult::GameState::NORMAL);
  EXPECT_NO_THROW(board.move("F2", "F3"));
}

TEST_F(BoardTest, canUndoCheckmate) {
  board.move("E2", "E4");
  board.move("F7", "F6");
  board.move("D2", "D3");
  board.move("G7", "G5");
  auto state = board.move("D1", "H5").gameState();
  EXPECT_EQ(state, MoveResult::GameState::OPPONENT_IN_CHECKMATE);
  EXPECT_TRUE(board.isGameOver());

  board.undoLastMove();
  state = board.move("A2", "A3").gameState();
  EXPECT_EQ(state, MoveResult::GameState::NORMAL);
  EXPECT_FALSE(board.isGameOver());
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

TEST_F(BoardTest, hashIsGeneratedDuringAMove) {
  auto hasher = buildNiceBoardHasherMock();
  auto& mock = *hasher;
  board = Board(std::move(hasher));
  EXPECT_CALL(mock, hash()).Times(AtLeast(1));
  board.move("A2", "A3");
}

TEST_F(BoardTest, hasherIsNotifiedOfMove) {
  auto hasher = buildNiceBoardHasherMock();
  auto& mock = *hasher;
  board = Board(std::move(hasher));
  board.move("A2", "A4"); board.move("B7", "B5");
  EXPECT_CALL(mock, pieceMoved(Coordinates(0, 3), Coordinates(1, 4)));
  EXPECT_CALL(mock, togglePlayer());
  board.move("A4", "B5");
}

TEST_F(BoardTest, hasherIsResetWithBoard) {
  auto hasher = buildNiceBoardHasherMock();
  auto& mock = *hasher;
  board = Board(std::move(hasher));
  EXPECT_CALL(mock, reset());
  board.reset();
}

TEST_F(BoardTest, previousHashIsRestoredWhenAMoveIsUndone) {
  auto hasher = buildNiceBoardHasherMock();
  auto& mock = *hasher;
  board = Board(std::move(hasher));
  board.move("A2", "A3");
  EXPECT_CALL(mock, restorePreviousHash()).Times(AtLeast(1));
  board.undoLastMove();
}

TEST_F(BoardTest, hasherIsNotifiedOfPromotion) {
  auto hasher = buildNiceBoardHasherMock();
  auto& mock = *hasher;
  board = Board(std::move(hasher));
  decltype(mock.hash()) callCount = 0;
  ON_CALL(mock, hash())
    .WillByDefault(testing::Invoke(
        [&callCount]() { return callCount++; }
    ));
  movePawnsForPromotion(board);
  board.move("C7", "B8");

  EXPECT_CALL(mock, replacedWithPromotion(Coordinates(1, 7),
                                          PromotionOption::Queen,
                                          Chess::Colour::White));
  EXPECT_CALL(mock, togglePlayer());
  board.promote(PromotionOption::Queen);
}

TEST_F(BoardTest, hasherIsNotifiedOfCastling) {
  auto hasher = buildNiceBoardHasherMock();
  auto& mock = *hasher;
  board = Board(std::move(hasher));
  decltype(mock.hash()) callCount = 0;
  ON_CALL(mock, hash())
    .WillByDefault(testing::Invoke(
        [&callCount]() { return callCount++; }
    ));
  board.move("G1", "F3"); board.move("G8", "F6");
  board.move("G2", "G3"); board.move("G7", "G6");
  board.move("F1", "G2"); board.move("F8", "G7");
  EXPECT_CALL(mock, pieceMoved(Coordinates(4, 0), Coordinates(6, 0))); // king
  EXPECT_CALL(mock, pieceMoved(Coordinates(7, 0), Coordinates(5, 0))); // rook
  EXPECT_CALL(mock, togglePlayer());
  board.move("E1", "G1");
}

TEST_F(BoardTest, hasherIsNotifiedOfCapturedPieceDuringEnPassant) {
  auto hasher = buildNiceBoardHasherMock();
  auto& mock = *hasher;
  board = Board(std::move(hasher));
  board.move("E2", "E4"); board.move("H7", "H5");
  board.move("E4", "E5"); board.move("D7", "D5");
  EXPECT_CALL(mock, removed(Coordinates(3, 4)));
  EXPECT_CALL(mock, togglePlayer());
  board.move("E5", "D6");
}

TEST_F(BoardTest, boardCanBeInstantiatedWithANonStandardInitialConfiguration) {
  board = Board({}, {Coordinates(2, 3), Coordinates(1, 2),
    Coordinates(2, 2)}, {}, {}, {}, Coordinates(1,1), {}, {}, {}, {},
    {}, Coordinates(7,7));

  EXPECT_TRUE(board.at(Coordinates(0, 0)) == nullptr);
  auto whiteRook = board.at(Coordinates(2, 3));
  ASSERT_FALSE(whiteRook == nullptr);
  auto anotherWhiteRook = board.at(Coordinates(1, 2));
  ASSERT_FALSE(anotherWhiteRook == nullptr);
  EXPECT_TRUE(whiteRook->name() == anotherWhiteRook->name());

  EXPECT_FALSE(board.at(Coordinates(2, 2)) == nullptr);
  EXPECT_FALSE(board.at(Coordinates(1, 1)) == nullptr);
  EXPECT_FALSE(board.at(Coordinates(7, 7)) == nullptr);

  EXPECT_FALSE(board.isGameOver());
}

TEST_F(BoardTest, throwsIfNonStandardInitialisationHasRepeatedCoordinates) {
  EXPECT_THROW(board = Board({}, {Coordinates(2, 3), Coordinates(2, 3),
    Coordinates(2, 2)}, {}, {}, {}, Coordinates(1,1), {}, {}, {}, {},
    {}, Coordinates(7,7));, std::invalid_argument);

  EXPECT_THROW(board = Board({}, {Coordinates(2, 3), Coordinates(1, 2),
    Coordinates(2, 2)}, {}, {}, {}, Coordinates(1,1), {}, {}, {}, {},
    {}, Coordinates(1,2));, std::invalid_argument);
}

TEST_F(BoardTest, throwsIfNonStandardInitialisationHasInvalidCoordinates) {
  EXPECT_THROW(board = Board({}, {Coordinates(-2, 3), Coordinates(2, 3),
    Coordinates(2, 2)}, {}, {}, {}, Coordinates(1,1), {}, {}, {}, {},
    {}, Coordinates(7,7));, std::invalid_argument);

  EXPECT_THROW(board = Board({}, {Coordinates(2, -3), Coordinates(2, 3),
    Coordinates(2, 2)}, {}, {}, {}, Coordinates(1,1), {}, {}, {}, {},
    {}, Coordinates(7,7));, std::invalid_argument);

  EXPECT_THROW(board = Board({}, {Coordinates(2, Board::MAX_ROW_NUM + 1),
    Coordinates(1, 2), Coordinates(2, 2)}, {}, {}, {}, Coordinates(1,1),
    {}, {}, {}, {}, {}, Coordinates(1,2));, std::invalid_argument);

  EXPECT_THROW(board = Board({}, {Coordinates(Board::MAX_COL_NUM+1, 3),
    Coordinates(1, 2), Coordinates(2, 2)}, {}, {}, {}, Coordinates(1,1),
    {}, {}, {}, {}, {}, Coordinates(1,2));, std::invalid_argument);
}

TEST_F(BoardTest, throwsIfMoreThanOnePromotionWhenNonStandardInitialised) {
  EXPECT_THROW(board = Board({}, {}, {}, {}, {}, Coordinates(1, 1),
    {Coordinates(0, 0), Coordinates(4, 0)}, {}, {}, {},
    {}, Coordinates(7, 7));, std::invalid_argument);

  EXPECT_THROW(board = Board({Coordinates(5, 7)}, {}, {}, {}, {},
    Coordinates(1, 1), {Coordinates(0, 0)}, {}, {}, {}, {},
    Coordinates(7, 7));, std::invalid_argument);
}

TEST_F(BoardTest, rooksInitialisedInNonStandardPositionHaveMovedStatusTrue) {
  board = Board({}, {Coordinates(2, 3)}, {}, {}, {}, Coordinates(1,1),
    {}, {Coordinates(7,6)}, {}, {}, {}, Coordinates(7,7));
  auto whiteRook = board.at(Coordinates(2, 3));
  ASSERT_FALSE(whiteRook == nullptr);
  EXPECT_TRUE(whiteRook->getMovedStatus());

  auto blackRook = board.at(Coordinates(7, 6));
  ASSERT_FALSE(blackRook == nullptr);
  EXPECT_TRUE(blackRook->getMovedStatus());
}

TEST_F(BoardTest, rooksInitialisedInStandardPositionHaveMovedStatusFalse) {
  board = Board({}, {Coordinates(0, 0)}, {}, {}, {}, Coordinates(1, 1),
    {}, {Coordinates(7, 7)}, {}, {}, {}, Coordinates(7, 4));
  auto whiteRook = board.at(Coordinates(0, 0));
  ASSERT_FALSE(whiteRook == nullptr);
  EXPECT_FALSE(whiteRook->getMovedStatus());

  auto blackRook = board.at(Coordinates(7, 7));
  ASSERT_FALSE(blackRook == nullptr);
  EXPECT_FALSE(blackRook->getMovedStatus());
}

TEST_F(BoardTest, pawnsInitialisedInNonStandardPositionHaveMovedStatusTrue) {
  board = Board({Coordinates(2, 3)}, {}, {}, {}, {}, Coordinates(1,1),
    {Coordinates(7,5)}, {}, {}, {}, {}, Coordinates(7,7));
  auto whitePawn = board.at(Coordinates(2, 3));
  ASSERT_FALSE(whitePawn == nullptr);
  EXPECT_TRUE(whitePawn->getMovedStatus());

  auto blackPawn = board.at(Coordinates(7, 5));
  ASSERT_FALSE(blackPawn == nullptr);
  EXPECT_TRUE(blackPawn->getMovedStatus());
}

TEST_F(BoardTest, pawnsInitialisedInStandardPositionHaveMovedStatusFalse) {
  board = Board({Coordinates(0, 1)}, {}, {}, {}, {}, Coordinates(1,1),
    {Coordinates(7,6)}, {}, {}, {}, {}, Coordinates(7,7));
  auto whitePawn = board.at(Coordinates(0, 1));
  ASSERT_FALSE(whitePawn == nullptr);
  EXPECT_FALSE(whitePawn->getMovedStatus());

  auto blackPawn = board.at(Coordinates(7, 6));
  ASSERT_FALSE(blackPawn == nullptr);
  EXPECT_FALSE(blackPawn->getMovedStatus());
}

TEST_F(BoardTest, kingsInitialisedInNonStandardPositionHaveMovedStatusTrue) {
  board = Board({Coordinates(2, 3)}, {}, {}, {}, {}, Coordinates(1,1),
    {Coordinates(7,6)}, {}, {}, {}, {}, Coordinates(7,7));
  auto whiteKing = board.at(Coordinates(1, 1));
  ASSERT_FALSE(whiteKing == nullptr);
  EXPECT_TRUE(whiteKing->getMovedStatus());

  auto blackKing = board.at(Coordinates(7, 7));
  ASSERT_FALSE(blackKing == nullptr);
  EXPECT_TRUE(blackKing->getMovedStatus());
}

TEST_F(BoardTest, kingsInitialisedInStandardPositionHaveMovedStatusFalse) {
  board = Board({Coordinates(2, 3)}, {}, {}, {}, {}, Coordinates(4,0),
    {Coordinates(7,6)}, {}, {}, {}, {}, Coordinates(4,7));
  auto whiteKing = board.at(Coordinates(4, 0));
  ASSERT_FALSE(whiteKing == nullptr);
  EXPECT_FALSE(whiteKing->getMovedStatus());

  auto blackKing = board.at(Coordinates(4, 7));
  ASSERT_FALSE(blackKing == nullptr);
  EXPECT_FALSE(blackKing->getMovedStatus());
}

TEST_F(BoardTest, pawnsInitialisedInEnPassantCannotExecuteIt) {
  board = Board({Coordinates(2, 4)}, {}, {}, {}, {}, Coordinates(1, 1),
  {Coordinates(3, 4)}, {}, {}, {}, {}, Coordinates(7, 7));
  auto whitePawn = board.at(Coordinates(2, 4));
  ASSERT_FALSE(whitePawn == nullptr);
  EXPECT_FALSE(whitePawn->isNormalMove(Coordinates(2,4), Coordinates(3,5)));
}

TEST_F(BoardTest, kingVsKingCausesDraw) {
  board = Board({}, {}, {}, {}, {}, Coordinates(1,1),
                {Coordinates(1,2)}, {}, {}, {}, {}, Coordinates(7,7));
  auto result = board.move(Coordinates(1,1), Coordinates(1,2));
  EXPECT_EQ(result.gameState(),
            MoveResult::GameState::INSUFFICIENT_MATERIAL_DRAW);
  EXPECT_TRUE(board.isGameOver());
}

TEST_F(BoardTest, kingAndBishopVsKingAndBishopCausesDraw) {
  board = Board({}, {}, {}, {Coordinates(0, 3)}, {}, Coordinates(1, 1),
    {Coordinates(1, 4)}, {}, {}, {Coordinates(7, 3)}, {}, Coordinates(7, 7));
  auto result = board.move(Coordinates(0,3), Coordinates(1,4));
  EXPECT_EQ(result.gameState(),
            MoveResult::GameState::INSUFFICIENT_MATERIAL_DRAW);
  EXPECT_TRUE(board.isGameOver());
}

TEST_F(BoardTest, kingAndKnightVsKingAndKnightCausesDraw) {
  board = Board({}, {}, {Coordinates(0,0)}, {}, {}, Coordinates(1,1),
         {Coordinates(1,2)}, {}, {Coordinates(7,7)}, {}, {}, Coordinates(7,6));
  auto result = board.move(Coordinates(1,1), Coordinates(1,2));
  EXPECT_EQ(result.gameState(),
            MoveResult::GameState::INSUFFICIENT_MATERIAL_DRAW);
  EXPECT_TRUE(board.isGameOver());
}

TEST_F(BoardTest, kingAndKnightVsKingAndBishopCausesDraw) {
  // white knight vs black bishop
  board = Board({}, {}, {Coordinates(0,0)}, {}, {}, Coordinates(1,1),
       {Coordinates(1,2)}, {}, {}, {Coordinates(0, 7)}, {}, Coordinates(7, 6));
  auto result = board.move(Coordinates(1,1), Coordinates(1,2));
  EXPECT_EQ(result.gameState(),
            MoveResult::GameState::INSUFFICIENT_MATERIAL_DRAW);
  EXPECT_TRUE(board.isGameOver());

  // white bishop vs black knight
  board = Board({Coordinates(7,4)}, {}, {}, {Coordinates(0,0)}, {},
    Coordinates(1,1), {}, {}, {Coordinates(7,7)}, {}, {}, Coordinates(7,6));
  board.move(Coordinates(7,4), Coordinates(7,5));
  result = board.move(Coordinates(7,6), Coordinates(7,5));
  EXPECT_EQ(result.gameState(),
            MoveResult::GameState::INSUFFICIENT_MATERIAL_DRAW);
  EXPECT_TRUE(board.isGameOver());
}

TEST_F(BoardTest, callingResetResetsTheInternalState) {
  testAlekhineVsVasic1931();
  board.reset();
  testAlekhineVsVasic1931();
}

TEST_F(BoardTest, moveAssignmentOverwritesTheInternalState) {
  testAlekhineVsVasic1931();
  board = Board();
  testAlekhineVsVasic1931();
}