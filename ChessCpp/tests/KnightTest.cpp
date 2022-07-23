#include "pch.h"
#include "Board.hpp"

using Chess::Piece;
using Chess::Coordinates;
using Chess::Board;
using Chess::MoveResult;
using Chess::InvalidMove;

static auto constexpr KNIGHT_COORD = Coordinates(3,3);

class KnightTest : public ::testing::Test {
protected:
  Board board = Board({Coordinates(7,6)}, {}, {KNIGHT_COORD},
               {}, {}, Coordinates(5,1), {}, {}, {}, {}, {}, Coordinates(5,7));
  Piece const* knight = nullptr;

  void SetUp() {
    knight = board.at(KNIGHT_COORD);
    ASSERT_FALSE(knight == nullptr);
    ASSERT_FALSE(board.isGameOver());
  }
};

TEST_F(KnightTest, canMoveInLShapeOneRightTwoUp) {
  EXPECT_TRUE(knight->isNormalMove(KNIGHT_COORD, Coordinates(4, 5)));
  EXPECT_NO_THROW(board.move(KNIGHT_COORD, Coordinates(4, 5)));
}

TEST_F(KnightTest, canMoveInLShapeOneLeftTwoUp) {
  EXPECT_TRUE(knight->isNormalMove(KNIGHT_COORD, Coordinates(2, 5)));
  EXPECT_NO_THROW(board.move(KNIGHT_COORD, Coordinates(2, 5)));
}

TEST_F(KnightTest, canMoveInLShapeOneRightTwoDown) {
  EXPECT_TRUE(knight->isNormalMove(KNIGHT_COORD, Coordinates(4, 1)));
  EXPECT_NO_THROW(board.move(KNIGHT_COORD, Coordinates(4, 1)));
}
 
TEST_F(KnightTest, canMoveInLShapeOneLeftTwoDown) {
  EXPECT_TRUE(knight->isNormalMove(KNIGHT_COORD, Coordinates(2, 1)));
  EXPECT_NO_THROW(board.move(KNIGHT_COORD, Coordinates(2, 1)));
}

TEST_F(KnightTest, canMoveInLShapeTwoRightOneUp) {
  EXPECT_TRUE(knight->isNormalMove(KNIGHT_COORD, Coordinates(5, 4)));
  EXPECT_NO_THROW(board.move(KNIGHT_COORD, Coordinates(5, 4)));
}

TEST_F(KnightTest, canMoveInLShapeTwoRightOneDown) {
  EXPECT_TRUE(knight->isNormalMove(KNIGHT_COORD, Coordinates(5, 2)));
  EXPECT_NO_THROW(board.move(KNIGHT_COORD, Coordinates(5, 2)));
}

TEST_F(KnightTest, canMoveInLShapeTwoLeftOneUp) {
  EXPECT_TRUE(knight->isNormalMove(KNIGHT_COORD, Coordinates(1, 4)));
  EXPECT_NO_THROW(board.move(KNIGHT_COORD, Coordinates(1, 4)));
}

TEST_F(KnightTest, canMoveInLShapeTwoLeftOneDown) {
  EXPECT_TRUE(knight->isNormalMove(KNIGHT_COORD, Coordinates(1, 2)));
  EXPECT_NO_THROW(board.move(KNIGHT_COORD, Coordinates(1, 2)));
}

TEST_F(KnightTest, cannotMoveInStraightLines) {
  EXPECT_FALSE(knight->isNormalMove(KNIGHT_COORD, Coordinates(3, 5)));
  EXPECT_THROW(board.move(KNIGHT_COORD, Coordinates(3, 5)), InvalidMove);

  EXPECT_FALSE(knight->isNormalMove(KNIGHT_COORD, Coordinates(3, 1)));
  EXPECT_THROW(board.move(KNIGHT_COORD, Coordinates(3, 1)), InvalidMove);

  EXPECT_FALSE(knight->isNormalMove(KNIGHT_COORD, Coordinates(5, 3)));
  EXPECT_THROW(board.move(KNIGHT_COORD, Coordinates(5, 3)), InvalidMove);

  EXPECT_FALSE(knight->isNormalMove(KNIGHT_COORD, Coordinates(1, 3)));
  EXPECT_THROW(board.move(KNIGHT_COORD, Coordinates(1, 3)), InvalidMove);
}

TEST_F(KnightTest, canMoveInLShapeWhileSteppingOverOtherPieces) {
  board = Board();
  auto wKnight = board.at(Coordinates(1, 0));
  EXPECT_TRUE(wKnight->isNormalMove(Coordinates(1, 0), Coordinates(2, 2)));
  EXPECT_TRUE(wKnight->isNormalMove(Coordinates(1, 0), Coordinates(0, 2)));

  board.move("B1", "C3");
  EXPECT_TRUE(wKnight->isNormalMove(Coordinates(2, 2), Coordinates(4, 3)));
  EXPECT_TRUE(wKnight->isNormalMove(Coordinates(2, 2), Coordinates(0, 3)));
}