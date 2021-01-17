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

namespace Chess {
Coordinates Board::stringToCoordinates(std::string const& coord) {
  if (coord.size() != 2)
    throw std::invalid_argument(coord + std::string(" is an invalid"
          " coordinate pair. Size must be 2."));
  if (coord[0] < MIN_COLUMN || coord[0] > MAX_COLUMN)
    throw std::out_of_range(coord + std::string(" is an invalid "
        "coordinate pair. Column must be within ") +
        MIN_COLUMN + std::string(" and ") + MAX_COLUMN);
  if (coord[1] < MIN_ROW || coord[1] > MAX_ROW)
    throw std::out_of_range(coord + std::string(" is an invalid "
        "coordinate pair. Row must be within ") +
        MIN_ROW + std::string(" and ") + MAX_ROW);

  // all good if we get here
  return Coordinates((int)(coord[0] - MIN_COLUMN), (int)(coord[1] - MIN_ROW));
}

std::string Board::coordinatesToString(Coordinates const& coord) {
  if (coord.column > MAX_COL_NUM || coord.row > MAX_ROW_NUM ||
      coord.column < 0 || coord.row < 0) {
    throw std::out_of_range("Coordinates are beyond the board limits");
  }
  return std::string(1, (char)(coord.column + MIN_COLUMN)) +
         std::string(1, (char)(coord.row + MIN_ROW));
}


// coordinates are ordered pairs of integers (column, row)
bool Board::areWithinLimits(Coordinates const& coord) {
  char row = (char)coord.row + MIN_ROW;
  char col = (char)coord.column + MIN_COLUMN;
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

Piece::Colour Board::currentPlayer() const {
  return isWhiteTurn ? Piece::Colour::White : Piece::Colour::Black;
}

bool Board::isGameOver() const {
  return m_isGameOver;
}

Board::Board():
  zobrist(ZobristHasher(Board::MAX_COL_NUM+1, Board::MAX_ROW_NUM+1)) {
  initializePieces();
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

void Board::initializePieces() {
  // make white and then black pieces
  for (int i = 0; i <= 1; i++) {
    auto colour = static_cast<Piece::Colour>(i);
    int row = MAX_ROW_NUM % (MAX_ROW_NUM + i); // can give 0 or MAX_ROW_NUM

    // not a memory leak ("board" takes ownership of all pieces)
    auto knight = new Knight(colour, *this);
    insufficientMaterial.emplace(*knight);
    board.emplace(Coordinates(1, row), knight);

    auto bishop = new Bishop(colour, *this);
    insufficientMaterial.emplace(*bishop);
    board.emplace(Coordinates(2, row), bishop);

    auto king = new King(colour, *this);
    board.emplace(Coordinates(4, row), king);
    insufficientMaterial.emplace(*king);
    kings.emplace(colour, *king);

    bishop = new Bishop(colour, *this);
    insufficientMaterial.emplace(*bishop);
    board.emplace(Coordinates(5, row), bishop);

    knight = new Knight(colour, *this);
    insufficientMaterial.emplace(*knight);
    board.emplace(Coordinates(6, row), knight);

    board.emplace(Coordinates(0, row), new Rook(colour, *this));
    board.emplace(Coordinates(3, row), new Queen(colour, *this));
    board.emplace(Coordinates(7, row), new Rook(colour, *this));

    row = (row == 0) ? 1 : MAX_ROW_NUM - 1;
    for (int c = 0; c <= MAX_COL_NUM; c++) {
      board.emplace(Coordinates(c, row), new Pawn(colour, *this));
    }
  }
}

void Board::reset() {
  countSincePawnMoveOrCapture = 0;
  zobrist.reset();
  isWhiteTurn = true;
  m_isGameOver = false;
  threeFoldRepetition = false;
  board.clear();
  kings.clear();
  movesHistory = std::stack<PastMove>();
  insufficientMaterial.clear();
  initializePieces();
}

MoveResult Board::move(std::string const& src, std::string const& destination) {
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
    throw std::logic_error("Piece is not a the specified source coordinates");
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
          movesHistory.emplace(source, destination,
                               board[source]->getMovedStatus(),
                               board[source]->getColour(),
                               std::move(board[toCapture]), toCapture);
          board.erase(toCapture);
          board[source]->setMovedStatus(true);
          board[destination] = std::move(board[source]);
          board.erase(source);
      } else {
        recordAndMove(source, destination);
      }

      if ((piece.getColour() == Piece::Colour::White &&
                  destination.row == MAX_ROW_NUM) ||
          (piece.getColour() == Piece::Colour::Black &&
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
    undoLastMove();
    countSincePawnMoveOrCapture = tmpCount;
    ss << (isWhiteTurn? "White" : "Black") <<
            "'s move is invalid as they would be in check";
    throw InvalidMove(ss.str(), InvalidMove::ErrorCode::CHECK_ERROR);
  }

  zobrist.pieceHasMoved(source, destination);
  std::optional<std::string> capturedPieceName;
  auto& lastMove = movesHistory.top();
  if (lastMove.wasAPieceRemoved()) {
    countSincePawnMoveOrCapture = 0;
    capturedPieceName = lastMove.getRemovedPiece().getName();
  }

  if (lastMove.destination() != lastMove.removedPieceCoords()) { // en passant
    zobrist.remove(lastMove.removedPieceCoords());
  }

  if (isPromotionPending()) {
    gameState = MoveResult::GameState::AWAITING_PROMOTION;
  } else {
    zobrist.togglePlayer();
    auto hash = zobrist.hash();
    storeBoardHash(hash);
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
  if ((pieceColour == Piece::Colour::Black && isWhiteTurn) ||
    (pieceColour == Piece::Colour::White && !isWhiteTurn)) {
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
  Piece::Colour enemyColour;
  enemyColour = isWhiteTurn ? Piece::Colour::Black : Piece::Colour::White;

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
  } else if (boardHashCount.count(zobrist.hash()) &&
                                      boardHashCount.at(zobrist.hash()) >= 5) {
    m_isGameOver = true;
    return MoveResult::GameState::FIVEFOLD_REPETITION_DRAW;
  } else if (inCheck && hasMoves) {
    return MoveResult::GameState::OPPONENT_IN_CHECK;
  } else if (!isMaterialSufficient()) {
    return MoveResult::GameState::INSUFFICIENT_MATERIAL_DRAW;
  }
  return MoveResult::GameState::NORMAL;
}

void Board::togglePlayer() {
  isWhiteTurn = !isWhiteTurn;
}

std::optional<CastlingType> getCastlingType(Coordinates const& source,
                                            Coordinates const& target) {
  if (source != Board::WHITE_KING_INIT &&
                                       source != Board::BLACK_KING_INIT) {
    return std::nullopt;
  }

  // in castling, row is always the king's
  if (target.row != source.row)
    return std::nullopt;

  int colOffset = source.column - target.column;
  if (colOffset == -1*Board::CASTLE_DISTANCE) {
    return CastlingType::KingSide;
  } else if (colOffset == Board::CASTLE_DISTANCE) {
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
    undoLastMove(); // undoes both rook and king, as castling is one move
    return std::nullopt;
  }

  zobrist.pieceHasMoved(rookSource, rookTarget);
  zobrist.pieceHasMoved(source, target);
  zobrist.togglePlayer();
  storeBoardHash(zobrist.hash());
  return castlingType;
}

void Board::storeBoardHash(int hash) {
  if (boardHashCount.count(hash) == 0) {
    boardHashCount[hash] = 0;
  }
  boardHashCount[hash] += 1;
}

bool Board::isMaterialSufficient() const {
  if (board.size() <= 2) {
    return false;
  }

  std::vector<std::reference_wrapper<Piece>> whites;
  std::vector<std::reference_wrapper<Piece>> blacks;
  for (auto const& coordPiece : board) {
    auto& piece = *(coordPiece.second);
    if (piece.getColour() == Piece::Colour::White) {
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

bool Board::isColumnFree(Coordinates source, int limitRow) const {
  if (source.row == limitRow)
    throw std::invalid_argument("source row and limitRow cannot be equal");

  // figure out directionality
  int dir = (source.row > limitRow) ? -1: 1;

  for (int row = source.row + dir; row != limitRow; row += dir) {
    source.row = row;
    if (!areWithinLimits(source)) {
      throw std::invalid_argument("Coordinates go beyond the board limits");
    }
    if (getPieceAtCoordinates(source)) {
      return false;
    }
  }

  return true;
}

bool Board::isRowFree(Coordinates source, int limitCol) const {
  if (source.column == limitCol)
    throw std::invalid_argument("source column and limitCol cannot be equal");

  // figure out directionality
  int dir = (source.column > limitCol) ? -1 : 1;

  for (int column = source.column+dir; column != limitCol; column += dir) {
    source.column = column;
    if (!areWithinLimits(source)) {
      throw std::invalid_argument("Coordinates go beyond the board limits");
    }
    if (getPieceAtCoordinates(source)) {
      return false;
    }
  }

  return true;
}

bool Board::isDiagonalFree(Coordinates source,
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
  for (source = Coordinates(source.column + columnAdd, source.row + rowAdd);
                                                    source != destination;
                      source.row += rowAdd, source.column += columnAdd) {
    if (!areWithinLimits(source)) {
      throw std::invalid_argument("Coordinates go beyond the board limits");
    }
    if (getPieceAtCoordinates(source)) {
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
  for (auto const& pair : board) {
    if (pair.second.get() == &piece) {
      return pair.first;
    }
  }

  throw std::logic_error("Piece not found in chessboard");
}


bool Board::isKingInCheck(Piece::Colour kingColour) const {
  Coordinates kingCoord = getPieceCoordinates(kings.at(kingColour));

  for (auto const& pair : board) {
    Coordinates pieceCoord = pair.first;
    auto& piece = pair.second;

    // check if an enemy piece can move where the king is
    if (piece->getColour() != kingColour &&
      piece->isMovePlausible(pieceCoord, kingCoord)) {
      return true;
    }
  }

  return false;
}

bool Board::hasMovesLeft(Piece::Colour colour) {
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
     movesHistory.emplace(source, destination,
                          board[source]->getMovedStatus(),
                          board[source]->getColour(),
                          std::move(board[destination]));
  } else {
     movesHistory.emplace(source, destination,
                          board[source]->getMovedStatus(),
                          board[source]->getColour());
   }
   board[source]->setMovedStatus(true);
   board[destination] = std::move(board[source]);
   board.erase(source);
}

bool Board::isMoveSuicide(Coordinates source,
                             Coordinates destination) {
  recordAndMove(source, destination);
  bool check = isKingInCheck(board[destination]->getColour());
  undoLastMove();
  return check;
}

void Board::undoLastMove() {
  if (movesHistory.size() > 0) {
    auto& source = movesHistory.top().source();
    auto& dest = movesHistory.top().destination();
    auto srcMoved = movesHistory.top().sourceMovedStatus();
    if (getCastlingType(source, dest).has_value() && !srcMoved) {
      undoMoveInStackTop(); // castling is stored as two moves
    }
    undoMoveInStackTop();
    promotionSource.reset(); // promotion should be undone as well
  }
}

void Board::undoMoveInStackTop() {
  auto& lastMove = movesHistory.top();
  auto& source = lastMove.source();
  auto& dest = lastMove.destination();

  board[source] = std::move(board[dest]);
  board[source]->setMovedStatus(lastMove.sourceMovedStatus());
  if (lastMove.wasAPieceRemoved()) {
    board[dest] = std::move(lastMove.transferRemovedPiece());
  } else {
    board.erase(dest);
  }
  movesHistory.pop();
}

bool Board::isValidEnPassant(Pawn const& pawn, Coordinates const& source,
                                         Coordinates const& destination) const {
  if (!isPieceAtSource(pawn, source)) {
    return false;
  }

  if (movesHistory.empty() || movesHistory.top().sourceMovedStatus() ||
     pawn.getColour() == movesHistory.top().playerColour()) {
    return false;
  }

  auto& lastMoveDest = movesHistory.top().destination();
  if (source.row != lastMoveDest.row ||
      abs(source.column - lastMoveDest.column) != 1) {
    return false;
  }

  auto& lastMoveSrc = movesHistory.top().source();
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

MoveResult Board::promote(PromotionOption piece) {
  if (!promotionSource) {
    throw std::logic_error("There is no promotion pending");
  }

  auto& source = *promotionSource;
  auto moved = board[source]->getMovedStatus();
  auto colour = board[source]->getColour();
  movesHistory.emplace(source, source, moved,
                        colour, std::move(board.at(source)));
  board[source] = std::move(buildPromotionPiece(piece));
  if (piece == PromotionOption::Knight || piece == PromotionOption::Bishop) {
    insufficientMaterial.emplace(*board[source]);
  }

  promotionSource.reset();
  auto state = checkGameState();
  zobrist.replacedWithPromotion(source, piece, currentPlayer());
  togglePlayer();
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
    out << std::setw(Board::H_PRINT_SIZE) << "|";
  }

  out << "\n|";
  for (int j = 0; j <= Board::MAX_COL_NUM; j++) {
    for (int i = 0; i < Board::H_PRINT_SIZE - 1; i++) {
      out << '-';
    }
    out << '|';
  }
}

void printColumnLegend(std::ostream& out) {
  for (char ch = Board::MIN_COLUMN; ch <= Board::MAX_COLUMN; ch++) {
    out << std::setw((std::streamsize)Board::H_PRINT_SIZE / 2 + 1) << ch;
    out << std::setw(Board::H_PRINT_SIZE / 2) << " ";
  }
}

void printTopLine(std::ostream& out) {
  out << "\n|";
  for (int j = 0; j <= Board::MAX_COL_NUM; j++) {
    out << std::setw(Board::H_PRINT_SIZE) << '|';
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
        out << std::setw((std::streamsize)Board::H_PRINT_SIZE - 1);
        out << *pieceOptional << '|';
      } else {
        out << std::setw(Board::H_PRINT_SIZE) << "|";
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

}
