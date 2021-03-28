#include "Board.hpp"
#include "Rook.hpp"
#include "Bishop.hpp"
#include "Queen.hpp"
#include "Knight.hpp"
#include "King.hpp"
#include "Pawn.hpp"
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <array>
#include <type_traits>

/// Defines the starting position of the white king.
static auto constexpr WHITE_KING_INIT = Chess::Coordinates(4, 0);
/// Defines the starting position of the black king.
static auto constexpr BLACK_KING_INIT = Chess::Coordinates(4, 7);
/// Defines the number of squares the king travels to castle.
static int constexpr CASTLE_DISTANCE = 2;
/// Defines the horizontal printing space used for a square of the board.
static int constexpr H_PRINT_SIZE = 15;

namespace Chess {

struct Board::PastMove {
  PastMove(Board const& board,
           Coordinates const& source,
           Coordinates const& destination,
           bool sourceMoved,
           std::unique_ptr<Piece> removedPiece = nullptr):
                     PastMove(board, source, destination, sourceMoved,
                                      std::move(removedPiece), destination) {}

  PastMove(Board const& board,
           Coordinates const& source,
           Coordinates const& destination,
           bool sourceMoved,
           std::unique_ptr<Piece> capturedPiece,
           Coordinates const& capturedCoords):
              source(source),
              destination(destination), 
              sourceMovedStatus(sourceMoved),
              removedPieceCoords(capturedCoords),
              removedPiece(std::move(capturedPiece)),
              isWhiteTurn(board.isWhiteTurn),
              promotionSource(board.promotionSource),
              boardHashCount(board.boardHashCount),
              countSincePawnMoveOrCapture(board.countSincePawnMoveOrCapture),
              threeFoldRepetition(board.threeFoldRepetition),
              insufficientMaterial(board.insufficientMaterial) {}

  Coordinates source;
  Coordinates destination;
  bool sourceMovedStatus = false;

  Coordinates removedPieceCoords;
  std::unique_ptr<Piece> removedPiece = nullptr;

  bool isWhiteTurn = false;
  std::optional<Coordinates> promotionSource;
  std::unordered_map<int, size_t> boardHashCount;
  int countSincePawnMoveOrCapture = 0;
  bool threeFoldRepetition = false;
  std::unordered_set<std::reference_wrapper<Piece>,
                                          PieceRefHasher> insufficientMaterial;
};

Coordinates Board::stringToCoordinates(std::string_view coord) {
  if (coord.size() != 2) {
    throw std::invalid_argument(std::string(coord) + 
      std::string(" is an invalid coordinate pair. Size must be 2"));
  }

  if (coord[0] < MIN_COLUMN || coord[0] > MAX_COLUMN) {
    throw std::out_of_range(std::string(coord) +
      std::string(" is an invalid coordinate pair. Column must be within ") +
        MIN_COLUMN + std::string(" and ") + MAX_COLUMN);
  }

  if (coord[1] < MIN_ROW || coord[1] > MAX_ROW) {
    throw std::out_of_range(std::string(coord) +
      std::string(" is an invalid coordinate pair. Row must be within ") +
        MIN_ROW + std::string(" and ") + MAX_ROW);
  }

  return Coordinates(static_cast<int>(coord[0] - MIN_COLUMN),
                     static_cast<int>(coord[1] - MIN_ROW));
}

std::string Board::coordinatesToString(Coordinates const& coord) {
  if (coord.column > MAX_COL_NUM || coord.row > MAX_ROW_NUM ||
      coord.column < 0 || coord.row < 0) {
    throw std::out_of_range("Coordinates are beyond the board limits");
  }
  return std::string(1,static_cast<char>(coord.column + MIN_COLUMN)) +
         std::string(1, static_cast<char>(coord.row + MIN_ROW));
}

bool Board::areWithinLimits(Coordinates const& coord) {
  char row = static_cast<char>(coord.row) + MIN_ROW;
  char col = static_cast<char>(coord.column) + MIN_COLUMN;
  return (row >= MIN_ROW && row <= MAX_ROW &&
          col >= MIN_COLUMN && col <= MAX_COLUMN);
}

bool Board::areInSameRow(Coordinates const& coord1,
                              Coordinates const& coord2) {
  return (coord1.row == coord2.row);
}
bool Board::areInSameColumn(Coordinates const& coord1,
                                 Coordinates const& coord2) {
  return (coord1.column == coord2.column);
}
bool Board::areInSameDiagonal(Coordinates const& coord1,
                                   Coordinates const& coord2) {
  return (abs(coord1.column - coord2.column) ==
                             abs(coord1.row - coord2.row));
}

Colour Board::currentPlayer() const {
  return isWhiteTurn ? Colour::White : Colour::Black;
}

bool Board::isGameOver() const {
  return m_isGameOver;
}

Board::Board(): Board(std::make_unique<ZobristHasher>(Board::MAX_COL_NUM+1,
                                                      Board::MAX_ROW_NUM+1)) {}

Board::Board(std::unique_ptr<BoardHasher> hasher): hasher(std::move(hasher)) {
  if (this->hasher == nullptr) {
    throw std::invalid_argument("The board hasher cannot be null");
  }
  initializePiecesInStandardPos();
}

Board::Board(std::vector<Coordinates> const& whitePawns,
             std::vector<Coordinates> const& whiteRooks,
             std::vector<Coordinates> const& whiteKnights,
             std::vector<Coordinates> const& whiteBishops,
             std::vector<Coordinates> const& whiteQueens,
             Coordinates const& whiteKing,
             std::vector<Coordinates> const& blackPawns,
             std::vector<Coordinates> const& blackRooks,
             std::vector<Coordinates> const& blackKnights,
             std::vector<Coordinates> const& blackBishops,
             std::vector<Coordinates> const& blackQueens,
             Coordinates const& blackKing):
             hasher(std::make_unique<ZobristHasher>(Board::MAX_COL_NUM+1,
                                                    Board::MAX_ROW_NUM+1,
                                                     whitePawns, whiteRooks,
                                                     whiteKnights, whiteBishops,
                                                     whiteQueens, whiteKing,
                                                     blackPawns, blackRooks,
                                                     blackKnights, blackBishops,
                                                     blackQueens, blackKing)) {
  std::array<Colour, 2> colours = {Colour::White, Colour::Black};
  for (auto const& colour : colours) {
    initializePawns(colour == Colour::White ? whitePawns : blackPawns, colour);
    initializeRooks(colour == Colour::White ? whiteRooks : blackRooks, colour);
    initializeKnights(colour == Colour::White ? whiteKnights : blackKnights, colour);
    initializeBishops(colour == Colour::White ? whiteBishops : blackBishops, colour);
    initializeQueens(colour == Colour::White ? whiteQueens : blackQueens, colour);
    initializeKing(colour == Colour::White ? whiteKing : blackKing, colour);
  }
  checkGameState();
}

void Board::initializePawns(std::vector<Coordinates> const& coords,
                            Colour colour) {
  initializePieces<Pawn>(coords, colour,
          [](int column) { return column > Board::MAX_COL_NUM; });

  auto row = (colour == Colour::White) ? MAX_ROW_NUM : 0;
  for (auto const& coord : coords) {
    if (coord.row == row) {
      if (promotionSource.has_value()) {
        throw std::invalid_argument("Only one pawn can be positioned for "
                                    "promotion in any given turn");
      }
      promotionSource = coord;
    }
  }
}

void Board::initializeRooks(std::vector<Coordinates> const& coords,
                            Colour colour) {
  initializePieces<Rook>(coords, colour,
          [](int column) { return column == 0 || column == 7; });
}

void Board::initializeKnights(std::vector<Coordinates> const& coords,
                              Colour colour) {
  initializePieces<Knight>(coords, colour,
            [](int column) { return column == 1 || column == 6; },
    [this](auto& piece) { insufficientMaterial.emplace(piece); }
  );
}

void Board::initializeBishops(std::vector<Coordinates> const& coords,
                              Colour colour) {
  initializePieces<Bishop>(coords, colour,
    [](int column) { return column == 2 || column == 5; },
    [this](auto& piece) { insufficientMaterial.emplace(piece); }
  );
}

void Board::initializeQueens(std::vector<Coordinates> const& coords,
                             Colour colour) {
  initializePieces<Queen>(coords, colour,
        [](int column) { return column == 3; });
}

void Board::initializeKing(Coordinates const& coords, Colour colour) {
  initializePieces<King>({coords}, colour,
        [](int column) { return column == 4; },
        [&](King& king) {
            insufficientMaterial.emplace(king);
            kings.emplace(colour, king);
         });
}

template <typename Chessman, typename Predicate>
void Board::initializePieces(std::vector<Coordinates> const& coords,
                             Colour colour,
                             Predicate&& isNormalStartingColumn) {
  initializePieces<Chessman>(coords, colour, isNormalStartingColumn,
                             [](auto const&) {});
}

template <typename Chessman, typename Predicate, typename Finisher>
void Board::initializePieces(std::vector<Coordinates> const& coords,
                             Colour colour,
                             Predicate&& isNormalStartingColumn,
                             Finisher&& finalActions) {
  for (auto const& coord : coords) {
    if (!areWithinLimits(coord)) {
      throw std::invalid_argument("Coordinates go beyond the board limits");
    }
    if (board.count(coord)) {
      throw std::invalid_argument("Cannot initialize board with two or more"
                                  " pieces in the same coordinates");
    }

    auto chessman = std::make_unique<Chessman>(colour, *this);
    int initRow = (colour == Colour::White) ? 0 : MAX_ROW_NUM;
    if (!isNormalStartingColumn(coord.column) || coord.row != initRow) {
      chessman->setMovedStatus(true);
    }
    finalActions(*chessman);
    board.emplace(coord, std::move(chessman));
  }
}

bool Board::drawCanBeClaimed() const {
  // 50 moves rule is to be intended as 50 by each player, so 100 in total here
  return (threeFoldRepetition || countSincePawnMoveOrCapture >= 100) &&
                                                         !isPromotionPending();
}

void Board::claimDraw() {
  if (drawCanBeClaimed()) {
    m_isGameOver = true;
  }
}

void Board::initializePiecesInStandardPos() {
  // make white and then black pieces
  for (int i = 0; i <= 1; i++) {
    auto colour = static_cast<Colour>(i);
    int row = MAX_ROW_NUM % (MAX_ROW_NUM + i); // can give 0 or MAX_ROW_NUM

    auto knight = std::make_unique<Knight>(colour, *this);
    insufficientMaterial.emplace(*knight);
    board.emplace(Coordinates(1, row), std::move(knight));

    auto bishop = std::make_unique<Bishop>(colour, *this);
    insufficientMaterial.emplace(*bishop);
    board.emplace(Coordinates(2, row), std::move(bishop));

    auto king = std::make_unique<King>(colour, *this);
    insufficientMaterial.emplace(*king);
    kings.emplace(colour, *king);
    board.emplace(Coordinates(4, row), std::move(king));

    bishop = std::make_unique<Bishop>(colour, *this);
    insufficientMaterial.emplace(*bishop);
    board.emplace(Coordinates(5, row), std::move(bishop));

    knight = std::make_unique<Knight>(colour, *this);
    insufficientMaterial.emplace(*knight);
    board.emplace(Coordinates(6, row), std::move(knight));

    board.emplace(Coordinates(0, row), std::make_unique<Rook>(colour, *this));
    board.emplace(Coordinates(3, row), std::make_unique<Queen>(colour, *this));
    board.emplace(Coordinates(7, row), std::make_unique<Rook>(colour, *this));

    row = (row == 0) ? 1 : MAX_ROW_NUM - 1;
    for (int c = 0; c <= MAX_COL_NUM; c++) {
      board.emplace(Coordinates(c, row), std::make_unique<Pawn>(colour, *this));
    }
  }
}

void Board::reset() {
  countSincePawnMoveOrCapture = 0;
  hasher->reset();
  promotionSource.reset();
  boardHashCount.clear();
  isWhiteTurn = true;
  m_isGameOver = false;
  threeFoldRepetition = false;
  board.clear();
  kings.clear();
  movesHistory.clear();
  insufficientMaterial.clear();
  initializePiecesInStandardPos();
}

MoveResult Board::move(std::string_view src, std::string_view destination) {
  Coordinates sourceCoord;
  Coordinates targetCoord;
  try {
    sourceCoord = stringToCoordinates(src);
    targetCoord = stringToCoordinates(destination);
  } catch (std::exception const& e) {
    throw InvalidMove(e.what(), InvalidMove::ErrorCode::INVALID_COORDINATES);
  }

  return move(sourceCoord, targetCoord);
}

MoveResult Board::move(Coordinates const& src, Coordinates const& destination) {
  auto pieceOptional = getPieceAtCoordinates(src);

  if (!pieceOptional) {
    std::string sourceStr;
    try {
      sourceStr = Board::coordinatesToString(src);
    } catch (std::exception const& e) {
      throw InvalidMove(e.what(), InvalidMove::ErrorCode::INVALID_COORDINATES);
    }

    std::stringstream ss;
    ss << "There is no piece at position " << sourceStr;
    throw InvalidMove(ss.str(), InvalidMove::ErrorCode::NO_SOURCE_PIECE);
  }

  auto& piece = pieceOptional->get();
  return piece.move(src, destination);
}

bool Board::isPieceAtSource(Piece const& piece,
                                  Coordinates const& source) const {
  if (auto pieceOpt = getPieceAtCoordinates(source)) {
    auto& pieceAtSrc = pieceOpt->get();
    if (&piece == &pieceAtSrc) {
      return true;
    }
  }
  return false;
}

void Board::ensurePieceIsAtSource(Piece const& piece,
                                  Coordinates const& source) const {
  if (!isPieceAtSource(piece, source)) {
    throw std::logic_error("Piece is not at the specified source coordinates");
  }
}

MoveResult Board::move(Pawn& piece, Coordinates const& source,
                                    Coordinates const& destination) {
  ensurePieceIsAtSource(piece, source);
  return move(source, destination,
    [this, &piece] (Coordinates const& source, Coordinates const& destination) {
      if (isValidEnPassant(piece, source, destination)) {
          auto toCaptureRow = (destination.row == 2) ? 3 : MAX_ROW_NUM - 3;
          Coordinates toCapture(destination.column, toCaptureRow);
          movesHistory.emplace_back(*this, source, destination,
                               board[source]->getMovedStatus(),
                               std::move(board[toCapture]), toCapture);
          board.erase(toCapture);
          board[source]->setMovedStatus(true);
          board[destination] = std::move(board[source]);
          board.erase(source);
      } else {
        recordAndMove(source, destination);
      }

      if ((piece.getColour() == Colour::White &&
                  destination.row == MAX_ROW_NUM) ||
          (piece.getColour() == Colour::Black &&
                              destination.row == 0)) {
        promotionSource = destination;
      }
      countSincePawnMoveOrCapture = 0;
  });
}

MoveResult Board::move(PromotionPiece& piece, Coordinates const& source,
                                              Coordinates const& destination) {
  ensurePieceIsAtSource(piece, source);
  return move(source, destination,
    [this] (Coordinates const& source, Coordinates const& destination) {
      recordAndMove(source, destination);
      countSincePawnMoveOrCapture++;
    });
}

MoveResult Board::move(King& piece, Coordinates const& source, 
                                    Coordinates const& destination) {
  ensurePieceIsAtSource(piece, source);
  return move(source, destination,
    [this] (Coordinates const& source, Coordinates const& destination) {
      recordAndMove(source, destination);
      countSincePawnMoveOrCapture++;
    });
}

template <typename Callable>
MoveResult Board::move(Coordinates const& source,
                       Coordinates const& destination, Callable&& mover) {
  ensureGameNotOver();
  ensureNoPromotionNeeded();
  auto& piece = getPieceAtCoordinates(source)->get();
  ensurePlayerCanMovePiece(piece);
  auto gameState = MoveResult::GameState::NORMAL;

  if (auto castlingType = tryCastling(source, destination)) {
    countSincePawnMoveOrCapture++;
    gameState = checkGameState();
    togglePlayer();
    return MoveResult(gameState, *castlingType);
  }

  if (!piece.isMovePlausible(source, destination)) {
    std::string sourceStr, targetStr;
    try {
      sourceStr = coordinatesToString(source);
      targetStr = coordinatesToString(destination);
    } catch (std::exception const& e) {
      throw InvalidMove(e.what(), InvalidMove::ErrorCode::INVALID_COORDINATES);
    }

    std::stringstream ss;
    ss << piece << " cannot move from " << sourceStr << " to " << targetStr;
    throw InvalidMove(ss.str(), InvalidMove::ErrorCode::PIECE_LOGIC_ERROR);
  }

  // may need to restore count if move causes self check
  auto tmpCount = countSincePawnMoveOrCapture;
  mover(source, destination);

  if (isKingInCheck(currentPlayer())) {
    std::stringstream ss;
    promotionSource.reset();
    revertLastPieceMovement();
    movesHistory.pop_back();
    countSincePawnMoveOrCapture = tmpCount;
    ss << (isWhiteTurn? "White" : "Black") <<
            "'s move is invalid as they would be in check";
    throw InvalidMove(ss.str(), InvalidMove::ErrorCode::CHECK_ERROR);
  }

  auto& lastMove = movesHistory.back();
  if (lastMove.destination != lastMove.removedPieceCoords) { // en passant
    hasher->removed(lastMove.removedPieceCoords);
  }

  hasher->pieceMoved(source, destination);
  std::optional<std::string> capturedPieceName;
  if (lastMove.removedPiece != nullptr) {
    countSincePawnMoveOrCapture = 0;
    capturedPieceName = lastMove.removedPiece->getName();
  }

  if (isPromotionPending()) {
    gameState = MoveResult::GameState::AWAITING_PROMOTION;
  } else {
    hasher->togglePlayer();
    auto hash = hasher->hash();
    boardHashCount[hash]++;
    gameState = checkGameState();
    if (boardHashCount.at(hash) >= 3) {
      threeFoldRepetition = true;
    }
    togglePlayer();
  }

  if (capturedPieceName) {
    return MoveResult(gameState, *capturedPieceName);
  }
  return MoveResult(gameState);
}

void Board::ensureGameNotOver() {
  if (m_isGameOver) {
    throw InvalidMove("Game is already over, please reset",
                       InvalidMove::ErrorCode::GAME_OVER);
  }
}

void Board::ensurePlayerCanMovePiece(Piece const& piece) {
  auto pieceColour = piece.getColour();
  if ((pieceColour == Colour::Black && isWhiteTurn) ||
    (pieceColour == Colour::White && !isWhiteTurn)) {
    std::stringstream ss;
    ss << "It is not " << (isWhiteTurn ? "Black" : "White");
    ss << "'s turn to move";
    throw InvalidMove(ss.str(), InvalidMove::ErrorCode::WRONG_TURN);
  }
}

bool Board::isPromotionPending() const {
  return promotionSource.has_value();
}

void Board::ensureNoPromotionNeeded() {
  if (isPromotionPending()) {
    throw InvalidMove("Promote pawn before continuing",
                      InvalidMove::ErrorCode::PENDING_PROMOTION);
  }
}

MoveResult::GameState Board::checkGameState() {
  Colour enemyColour;
  enemyColour = isWhiteTurn ? Colour::Black : Colour::White;

  bool inCheck = isKingInCheck(enemyColour);
  bool hasMoves = hasMovesLeft(enemyColour);
  if (inCheck && !hasMoves) {
    m_isGameOver = true;
    return MoveResult::GameState::OPPONENT_IN_CHECKMATE;
  } else if (!inCheck && !hasMoves) {
    m_isGameOver = true;
    return MoveResult::GameState::STALEMATE;
  } else if (countSincePawnMoveOrCapture >= 150) { // 75 by each player
    m_isGameOver = true;
    return MoveResult::GameState::SEVENTYFIVE_MOVES_DRAW;
  } else if (boardHashCount.count(hasher->hash()) &&
                                      boardHashCount.at(hasher->hash()) >= 5) {
    m_isGameOver = true;
    return MoveResult::GameState::FIVEFOLD_REPETITION_DRAW;
  } else if (!isMaterialSufficient()) {
    m_isGameOver = true;
    return MoveResult::GameState::INSUFFICIENT_MATERIAL_DRAW;
  } else if (inCheck && hasMoves) {
    return MoveResult::GameState::OPPONENT_IN_CHECK;
  } 
  return MoveResult::GameState::NORMAL;
}

void Board::togglePlayer() {
  isWhiteTurn = !isWhiteTurn;
}

std::optional<CastlingType> getCastlingType(Coordinates const& source,
                                            Coordinates const& target) {
  if (source != WHITE_KING_INIT && source != BLACK_KING_INIT) {
    return std::nullopt;
  }

  // in castling, row is always the king's
  if (target.row != source.row)
    return std::nullopt;

  int colOffset = source.column - target.column;
  if (colOffset == -CASTLE_DISTANCE) {
    return CastlingType::KingSide;
  } else if (colOffset == CASTLE_DISTANCE) {
    return CastlingType::QueenSide;
  }
  return std::nullopt;
}


std::optional<CastlingType> Board::tryCastling(Coordinates const& source,
                                                   Coordinates const& target) {
  auto castlingTypeOpt = getCastlingType(source, target);
  if (!castlingTypeOpt) {
    return std::nullopt;
  }
  auto castlingType = castlingTypeOpt.value();

  int dir = 0; // column directionality for castling
  Coordinates rookTarget, rookSource;
  if (castlingType == CastlingType::KingSide) {
    rookSource = Coordinates(MAX_COL_NUM, source.row);
    rookTarget = Coordinates(MAX_COL_NUM - 2, source.row);
    dir = 1;
  } else {
    rookSource = Coordinates(0, source.row);
    rookTarget = Coordinates(3, source.row);
    dir = -1;
  }

  if (board.count(rookSource) <= 0 || board.count(source) <= 0)
    return std::nullopt;

  if (!isRowFree(source, target.column) || board.count(target) > 0)
    return std::nullopt;

  if (board[rookSource]->getMovedStatus() || board[source]->getMovedStatus())
    return std::nullopt;

  if (isKingInCheck(board[source]->getColour()))
    throw std::logic_error("Cannot castle as king is in check");

  // check if king's path is under attack
  for (auto coord = Coordinates(source.column + dir, source.row);
                           coord.column != target.column;
                           coord.column += dir) {
    if (isMoveSuicide(source, coord)) {
      return std::nullopt;
    }
  }

  // simulate castling and abort if it ends up in a check
  recordAndMove(rookSource, rookTarget);
  recordAndMove(source, target);
  if (isKingInCheck(board[target]->getColour())) {
    revertLastPieceMovement();
    movesHistory.pop_back();
    revertLastPieceMovement();
    movesHistory.pop_back();
    return std::nullopt;
  }

  hasher->pieceMoved(rookSource, rookTarget);
  hasher->pieceMoved(source, target);
  hasher->togglePlayer();
  boardHashCount[hasher->hash()]++;
  return castlingType;
}

bool Board::isMaterialSufficient() const {
  if (board.size() <= 2) {
    return false;
  }
  if (board.size() > 4) {
    return true;
  }

  std::vector<std::reference_wrapper<Piece>> whites;
  std::vector<std::reference_wrapper<Piece>> blacks;
  for (auto const& coordPiece : board) {
    auto& piece = *(coordPiece.second);
    if (piece.getColour() == Colour::White) {
      whites.emplace_back(piece);
    } else {
      blacks.emplace_back(piece);
    }
  }

  if (whites.size() > 2 || blacks.size() > 2) {
    return true;
  }

  for (auto const& white : whites) {
    if (insufficientMaterial.count(white) == 0) {
      return true;
    }
  }

  for (auto const& black : blacks) {
    if (insufficientMaterial.count(black) == 0) {
      return true;
    }
  }

  return false;
}

bool Board::isColumnFree(Coordinates const& source, int limitRow) const {
  if (source.row == limitRow)
    throw std::invalid_argument("source row and limitRow cannot be equal");

  // figure out directionality
  int dir = (source.row > limitRow) ? -1: 1;
  Coordinates src = source;
  for (int row = src.row + dir; row != limitRow; row += dir) {
    src.row = row;
    if (!areWithinLimits(src)) {
      throw std::invalid_argument("Coordinates go beyond the board limits");
    }
    if (getPieceAtCoordinates(src)) {
      return false;
    }
  }

  return true;
}

bool Board::isRowFree(Coordinates const& source, int limitCol) const {
  if (source.column == limitCol)
    throw std::invalid_argument("source column and limitCol cannot be equal");

  // figure out directionality
  int dir = (source.column > limitCol) ? -1 : 1;
  Coordinates src = source;
  for (int column = src.column+dir; column != limitCol; column += dir) {
    src.column = column;
    if (!areWithinLimits(src)) {
      throw std::invalid_argument("Coordinates go beyond the board limits");
    }
    if (getPieceAtCoordinates(src)) {
      return false;
    }
  }

  return true;
}

bool Board::isDiagonalFree(Coordinates const& source,
                                Coordinates const& destination) const {
  if (source == destination)
    throw std::invalid_argument("source and destination cannot be equal");
  if (!areInSameDiagonal(source, destination))
      throw std::invalid_argument("source and destination are"
                                 " not in the same diagonal");
  // figure out directionality
  int rowAdd = (source.row > destination.row)? -1 : 1;
  int columnAdd = (source.column > destination.column)? -1 : 1;

  // this will not check the extremes, as intended
  Coordinates src = source;
  for (src = Coordinates(src.column + columnAdd, src.row + rowAdd);
                                                    src != destination;
                      src.row += rowAdd, src.column += columnAdd) {
    if (!areWithinLimits(src)) {
      throw std::invalid_argument("Coordinates go beyond the board limits");
    }
    if (getPieceAtCoordinates(src)) {
      return false;
    }
  }

  return true;
}

OptionalRef<Piece> Board::getPieceAtCoordinates(
                                               Coordinates const& coord) const {
  if (board.count(coord) > 0) {
    return *(board.at(coord).get());
  }
  return std::nullopt;
}

Coordinates Board::getPieceCoordinates(Piece const& piece) const {
  for (auto const& [coord, otherPiece] : board) {
    if (otherPiece.get() == &piece) {
      return coord;
    }
  }

  throw std::logic_error("Piece not found in chessboard");
}


bool Board::isKingInCheck(Colour kingColour) const {
  Coordinates kingCoord = getPieceCoordinates(kings.at(kingColour));

  for (auto const& [coord, piece] : board) {
    // check if an enemy piece can move where the king is
    if (piece->getColour() != kingColour &&
      piece->isMovePlausible(coord, kingCoord)) {
      return true;
    }
  }

  return false;
}

bool Board::hasMovesLeft(Colour colour) {
  // copy keys as we need to edit the board in the loop
  std::vector<Coordinates> keys;
  for (auto const& keyVal : board) {
    keys.push_back(keyVal.first);
  }

  for (auto const& sourceCoord : keys) {
    if (board[sourceCoord]->getColour() != colour) {
      continue;
    }

    for (int r = 0; r <= MAX_ROW_NUM; r++) {
      for (int c = 0; c <= MAX_COL_NUM; c++) {
        Coordinates targetCoord(c,r);
        if (board[sourceCoord]->isMovePlausible(sourceCoord, targetCoord) &&
            !isMoveSuicide(sourceCoord, targetCoord)) {
           return true;
        }
      }
    }
  }

  // tried all possible moves and check is unavoidable
  return false;
}

void Board::recordAndMove(Coordinates const& source,
                               Coordinates const& destination) {
  if (board.count(source) <= 0) {
    throw std::logic_error("No piece in source coordinates");
  }
  if (!areWithinLimits(destination)) {
    throw std::invalid_argument("The destination is beyond the board limits");
  }

   if (board.count(destination) > 0) {
     movesHistory.emplace_back(*this, source, destination,
                          board[source]->getMovedStatus(),
                          std::move(board[destination]));
  } else {
     movesHistory.emplace_back(*this, source, destination,
                          board[source]->getMovedStatus());
   }
   board[source]->setMovedStatus(true);
   board[destination] = std::move(board[source]);
   board.erase(source);
}

bool Board::isMoveSuicide(Coordinates source,
                             Coordinates destination) {
  recordAndMove(source, destination);
  bool check = isKingInCheck(board[destination]->getColour());
  revertLastPieceMovement();
  movesHistory.pop_back();
  return check;
}

void Board::undoLastMove() {
  if (movesHistory.size() > 0) {
    auto srcMoved = movesHistory.back().sourceMovedStatus;
    auto castling = getCastlingType(movesHistory.back().source,
                                    movesHistory.back().destination);

    // castling and promotion are stored as 2 moves
    if ((castling.has_value() && !srcMoved) ||
         movesHistory.back().source == movesHistory.back().destination) {
      revertLastPieceMovement();
      movesHistory.pop_back();
      hasher->restorePreviousHash();
    }
    revertLastPieceMovement();

    auto& lastMove = movesHistory.back();
    m_isGameOver = false;
    isWhiteTurn = lastMove.isWhiteTurn;
    promotionSource = lastMove.promotionSource;
    boardHashCount = lastMove.boardHashCount;
    countSincePawnMoveOrCapture = lastMove.countSincePawnMoveOrCapture;
    threeFoldRepetition = lastMove.threeFoldRepetition;
    insufficientMaterial = lastMove.insufficientMaterial;

    movesHistory.pop_back();
    hasher->restorePreviousHash();
  }
}

void Board::revertLastPieceMovement() {
  auto& lastMove = movesHistory.back();
  auto& source = lastMove.source;
  auto& dest = lastMove.destination;

  board[source] = std::move(board[dest]);
  board[source]->setMovedStatus(lastMove.sourceMovedStatus);
  board.erase(dest);

  if (lastMove.removedPiece != nullptr) {
    Coordinates target = dest;
    if (lastMove.removedPieceCoords != lastMove.destination) { // en passant
      target = lastMove.removedPieceCoords;
    }
    board[target] = std::move(lastMove.removedPiece);
  }

}

bool Board::isValidEnPassant(Pawn const& pawn, Coordinates const& source,
                                         Coordinates const& destination) const {
  if (!isPieceAtSource(pawn, source) || movesHistory.empty()) {
    return false;
  }
  auto& lastMove = movesHistory.back();
  auto lastMoveColour = lastMove.isWhiteTurn ?
                        Colour::White : Colour::Black;
  if (lastMove.sourceMovedStatus || pawn.getColour() == lastMoveColour) {
    return false;
  }

  auto& lastMoveDest = lastMove.destination;
  if (source.row != lastMoveDest.row ||
      abs(source.column - lastMoveDest.column) != 1) {
    return false;
  }

  auto& lastMoveSrc = lastMove.source;
  if (destination.column != lastMoveSrc.column) {
    return false;
  }

  if (lastMoveSrc.row == 1 && lastMoveDest.row == 3 &&
      destination.row == 2) {
      return true;
  }

  if (lastMoveSrc.row == MAX_ROW_NUM - 1 &&
      lastMoveDest.row == MAX_ROW_NUM - 3 &&
      destination.row == MAX_ROW_NUM - 2) {
      return true;
  }

  return false;
}

std::optional<MoveResult> Board::promote(PromotionOption piece) {
  if (!promotionSource) {
    return std::nullopt;
  }

  auto& source = *promotionSource;
  auto moved = board[source]->getMovedStatus();
  movesHistory.emplace_back(*this, source, source,
                            moved, std::move(board[source]));
  board[source] = std::move(buildPromotionPiece(piece));
  if (piece == PromotionOption::Knight || piece == PromotionOption::Bishop) {
    insufficientMaterial.emplace(*board[source]);
  }

  promotionSource.reset();
  auto state = checkGameState();
  hasher->replacedWithPromotion(source, piece, currentPlayer());
  togglePlayer();
  hasher->togglePlayer();
  return MoveResult(state);
}

std::unique_ptr<PromotionPiece> Board::buildPromotionPiece(
                                                        PromotionOption piece) {
  switch (piece) {
  case PromotionOption::Queen:
    return std::make_unique<Queen>(currentPlayer(), *this);
  case PromotionOption::Knight:
    return std::make_unique<Knight>(currentPlayer(), *this);
  case PromotionOption::Bishop:
    return std::make_unique<Bishop>(currentPlayer(), *this);
  case PromotionOption::Rook:
    return std::make_unique<Rook>(currentPlayer(), *this);
  default:
    throw std::logic_error("Promotion not correctly implemented");
  }
}

void printBottomLines(std::ostream& out) {
  out << "\n|";
  for (int j = 0; j <= Board::MAX_COL_NUM; j++) {
    out << std::setw(H_PRINT_SIZE) << "|";
  }

  out << "\n|";
  for (int j = 0; j <= Board::MAX_COL_NUM; j++) {
    for (int i = 0; i < H_PRINT_SIZE - 1; i++) {
      out << '-';
    }
    out << '|';
  }
}

void printColumnLegend(std::ostream& out) {
  for (char ch = Board::MIN_COLUMN; ch <= Board::MAX_COLUMN; ch++) {
    out << std::setw((std::streamsize)H_PRINT_SIZE / 2 + 1) << ch;
    out << std::setw(H_PRINT_SIZE / 2) << " ";
  }
}

void printTopLine(std::ostream& out) {
  out << "\n|";
  for (int j = 0; j <= Board::MAX_COL_NUM; j++) {
    out << std::setw(H_PRINT_SIZE) << '|';
  }
  out << "\n|";
}

std::ostream& operator<<(std::ostream& out, Board const& board) {
  for (int r = board.MAX_ROW_NUM; r >= 0; r--) {
    printTopLine(out);

    for (int c = 0; c <= board.MAX_ROW_NUM; c++) {
      Coordinates iterCoord(c, r);
      if (auto pieceOptional = board.getPieceAtCoordinates(iterCoord)) {
        out << std::right;
        out << std::setw((std::streamsize)H_PRINT_SIZE - 1);
        out << *pieceOptional << '|';
      } else {
        out << std::setw(H_PRINT_SIZE) << "|";
      }
      if (c == board.MAX_ROW_NUM) {
        out << "  " << r+1;
      }
    }

    printBottomLines(out);
  }
  out << "\n";
  printColumnLegend(out);
  return out << "\n\n";
}

Board::~Board() = default;

}
