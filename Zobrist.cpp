#include "Zobrist.hpp"
#include <cstdlib>
#include <unordered_set>
#include <ctime>
#include <type_traits>

namespace Chess {
enum class ZobristHasher::PieceIndex : int {
  WhitePawn = 0, WhitePawnMoved, WhitePawnCanEnPassant,
  BlackPawn, BlackPawnMoved, BlackPawnCanEnPassant,
  WhiteKing, WhiteKingMoved,
  BlackKing, BlackKingMoved,
  WhiteQueen,
  BlackQueen,
  WhiteRook, WhiteRookMoved,
  BlackRook, BlackRookMoved,
  WhiteBishop,
  BlackBishop,
  WhiteKnight,
  BlackKnight
};

ZobristHasher::ZobristHasher(size_t width, size_t height):
    CHESSBOARD_AREA(width*height), MAX_ROW_NUM(height-1), MAX_COL_NUM(width-1),
    table(CHESSBOARD_AREA, std::vector<int>(PIECE_INDEXES_COUNT, EMPTY)),
    board(CHESSBOARD_AREA, EMPTY) {
  if (width <= 0 || height <= 0) {
    throw std::invalid_argument("Width and height must be positive");
  }
  reset();
}

void ZobristHasher::reset() {
  pawnsBeforeEnPassant.clear();
  initializeTableAndWhitePlayer();
  fillInitialBoard();
  currentHash = computeHashFromBoard();
}

void ZobristHasher::pieceHasMoved(Coordinates const& source,
                                  Coordinates const& destination) {
  auto src1D = to1D(source);
  if (board[src1D] != EMPTY) {
    auto dest1D = to1D(destination);
    auto movedVersion = movedEquivalent(
                        static_cast<ZobristHasher::PieceIndex>(board[src1D]));
      
    // reset en passant piece to what it was before
    // regardless of the move, the right to en passant is gone
    for (auto const& coordPiece : pawnsBeforeEnPassant) {
      replace(coordPiece.first, coordPiece.second);
    }
    pawnsBeforeEnPassant.clear();

    if (isEnPassantRow(destination.row)) {
      auto left = Coordinates(destination.column-1, destination.row);
      auto right = Coordinates(destination.column+1, destination.row);
      std::vector<int> coords1D;
      if (areWithinLimits(left)) coords1D.emplace_back(to1D(left));
      if (areWithinLimits(right)) coords1D.emplace_back(to1D(right));

      if (auto enemyPawnOpt = getEnemyMovedPawn(movedVersion)) {
        for (auto const& coord1D : coords1D) {
          if (board[coord1D] == static_cast<int>(*enemyPawnOpt)) {
            pawnsBeforeEnPassant[coord1D] = *enemyPawnOpt;
            replace(coord1D, *getEnPassantPawn(*enemyPawnOpt));
          }
        }
      }
    }
    replace(dest1D, movedVersion);
    remove(src1D);
  }
}

void ZobristHasher::remove(Coordinates const& coordDeletion) {
  remove(to1D(coordDeletion));
}

int ZobristHasher::hash() {
  return currentHash;
}

void ZobristHasher::replacedWithPromotion(Coordinates const& source,
                                   PromotionOption prom, Piece::Colour colour) {
  PieceIndex replacement;
  switch (prom) {
  case PromotionOption::Queen:
    replacement = (colour == Piece::Colour::White) ?
      PieceIndex::WhiteQueen : PieceIndex::BlackQueen; break;
  case PromotionOption::Bishop:
    replacement = (colour == Piece::Colour::White) ?
      PieceIndex::WhiteBishop : PieceIndex::BlackBishop; break;
  case PromotionOption::Knight:
    replacement = (colour == Piece::Colour::White) ?
      PieceIndex::WhiteKnight : PieceIndex::BlackKnight; break;
  case PromotionOption::Rook:
    replacement = (colour == Piece::Colour::White) ?
      PieceIndex::WhiteRook : PieceIndex::BlackRook; break;
  default:
    throw std::logic_error("Promotion not implemented correctly");
  }
  replace(to1D(source), replacement);
}

void ZobristHasher::initializeTableAndWhitePlayer() {
  auto seed = time(NULL);
  srand(static_cast<unsigned int>(seed));
  std::unordered_set<int> seen; // ensure unique random values
  for (auto& inner : table) {
    for (auto& bitstring : inner) {
      do { bitstring = rand(); } while (seen.count(bitstring) > 0);
      seen.insert(bitstring);
    }
  }
  do { whitePlayerHash = rand(); } while (seen.count(whitePlayerHash) > 0);
}

void ZobristHasher::fillInitialBoard() {
    // make white and then black pieces
  for (int i = 0; i <= 1; i++) {
    bool isBlack = static_cast<bool>(i);
    int row = MAX_ROW_NUM % (MAX_ROW_NUM+i); // can give 0 or MAX_ROW_NUM
    board[to1D(Coordinates(0, row))] =
          (int)(isBlack ? PieceIndex::BlackRook : PieceIndex::WhiteRook);
    board[to1D(Coordinates(1, row))] =
         (int)(isBlack ? PieceIndex::BlackKnight : PieceIndex::WhiteKnight);
    board[to1D(Coordinates(2, row))] =
         (int)(isBlack ? PieceIndex::BlackBishop : PieceIndex::WhiteBishop);
    board[to1D(Coordinates(3, row))] =
         (int)(isBlack ? PieceIndex::BlackQueen : PieceIndex::WhiteQueen);
    board[to1D(Coordinates(4, row))] =
         (int)(isBlack ? PieceIndex::BlackKing : PieceIndex::WhiteKing);
    board[to1D(Coordinates(5, row))] =
         (int)(isBlack ? PieceIndex::BlackBishop : PieceIndex::WhiteBishop);
    board[to1D(Coordinates(6, row))] =
         (int)(isBlack ? PieceIndex::BlackKnight : PieceIndex::WhiteKnight);
    board[to1D(Coordinates(7, row))] =
         (int)(isBlack ? PieceIndex::BlackRook : PieceIndex::WhiteRook);

    row = (row == 0)? 1 : MAX_ROW_NUM - 1;
    for (int c = 0; c <= MAX_COL_NUM; c++) {
        board[to1D(Coordinates(c, row))] =
           (int)(isBlack ? PieceIndex::BlackPawn : PieceIndex::WhitePawn);
    }
  }
}

int ZobristHasher::to1D(Coordinates const& coords) {
  if (!areWithinLimits(coords)) {
    throw std::out_of_range("Coordinates beyond hasher board limits");
  }
  return ((MAX_ROW_NUM + 1) * coords.row) + coords.column;
}

bool ZobristHasher::areWithinLimits(Coordinates const& coords) {
  return (coords.column <= MAX_COL_NUM && coords.column >= 0 &&
    coords.row <= MAX_ROW_NUM && coords.row >= 0);
}

int ZobristHasher::computeHashFromBoard() {
  int h = 0;
  for (size_t i = 0; i < board.size(); i++) {
    if (board[i] != EMPTY) {
      h ^= table[i][board[i]];
    }
  }
  return h;
}

bool ZobristHasher::isEnPassantRow(int row) {
  return row == MAX_ROW_NUM - 3 || row == 3;
}

void ZobristHasher::replace(int coor1D, ZobristHasher::PieceIndex replacement) {
  if (board[coor1D] != EMPTY) {
    currentHash ^= table[coor1D][board[coor1D]];
  }

  auto replacementIdx = static_cast<int>(replacement);
  board[coor1D] = replacementIdx;
  currentHash ^= table[coor1D][replacementIdx];
}

void ZobristHasher::remove(int coord1D) {
  if (board[coord1D] != EMPTY) {
    currentHash ^= table[coord1D][board[coord1D]];
    board[coord1D] = EMPTY;
  }
}

void ZobristHasher::togglePlayer() {
  currentHash ^= whitePlayerHash;
}

ZobristHasher::PieceIndex constexpr ZobristHasher::movedEquivalent(
                                                ZobristHasher::PieceIndex idx) {
    switch(idx) {
    case PieceIndex::WhitePawn:
      return PieceIndex::WhitePawnMoved;
    case PieceIndex::BlackPawn:
      return PieceIndex::BlackPawnMoved;

    case PieceIndex::WhiteKing:
      return PieceIndex::WhiteKingMoved;
    case PieceIndex::BlackKing:
      return PieceIndex::BlackKingMoved;

    case PieceIndex::WhiteRook:
      return PieceIndex::WhiteRookMoved;
    case PieceIndex::BlackRook:
      return PieceIndex::BlackRookMoved;

    default:
      return idx;
  }
}

std::optional<ZobristHasher::PieceIndex>
    ZobristHasher::getEnemyMovedPawn(ZobristHasher::PieceIndex pawn) {
    switch(pawn) {
    case PieceIndex::WhitePawnMoved:
      return PieceIndex::BlackPawnMoved;
    case PieceIndex::BlackPawnMoved:
      return PieceIndex::WhitePawnMoved;
    default:
      return std::nullopt;
  }
}

std::optional<ZobristHasher::PieceIndex>
     ZobristHasher::getEnPassantPawn(ZobristHasher::PieceIndex pawn) {
    switch(pawn) {
    case PieceIndex::WhitePawnMoved:
      return PieceIndex::WhitePawnCanEnPassant;
    case PieceIndex::BlackPawnMoved:
      return PieceIndex::BlackPawnCanEnPassant;
    default:
      return std::nullopt;
  }
}

}
