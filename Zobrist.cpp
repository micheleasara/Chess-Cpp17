#include "Zobrist.hpp"
#include "Rook.hpp"
#include "Bishop.hpp"
#include "Queen.hpp"
#include "Knight.hpp"
#include "King.hpp"
#include "Pawn.hpp"
#include <cstdlib>
#include <unordered_set>
#include <ctime>

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

struct ZobristHasher::PastMove {
  PastMove(ZobristHasher const& hasher,
           int src1D, int dest1D, int removedContent = EMPTY):
                    PastMove(hasher, src1D, dest1D, removedContent, dest1D) {}
  PastMove(ZobristHasher const& hasher,
           int src1D, int dest1D, int removedContent, int removedCoords):
            source(src1D),
            destination(dest1D),
            removedCoords(removedCoords),
            sourceContent(hasher.board[src1D]),
            destinationContent(hasher.board[dest1D]),
            removedContent(removedContent),
            pawnsBeforeEnPassant(hasher.pawnsBeforeEnPassant), 
            hashBeforeMove(hasher.currentHash) {}

  int source = 0;
  int destination = 0;
  int removedCoords = 0;
  int sourceContent = EMPTY;
  int destinationContent = EMPTY;
  int removedContent = EMPTY;
  std::unordered_map<int, PieceIndex> pawnsBeforeEnPassant;
  int hashBeforeMove = 0;
};

ZobristHasher::ZobristHasher() {
  initializeTableAndWhitePlayer();
  reset();
}

ZobristHasher::ZobristHasher(std::vector<Coordinates> const& whitePawns,
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
        Coordinates const& blackKing) {
  initializeTableAndWhitePlayer();
  initializePieces(whitePawns, whiteRooks, whiteKnights, whiteBishops,
                  whiteQueens, whiteKing, blackPawns, blackRooks, blackKnights,
                  blackBishops, blackQueens, blackKing);
  currentHash = computeHashFromBoard();
}

void ZobristHasher::reset() {
  movesHistory.clear();
  standardInitBoard();
  currentHash = computeHashFromBoard();
}

void ZobristHasher::pieceMoved(Coordinates const& source,
                                  Coordinates const& destination) {
   auto src1D = to1D(source);
   if (board[src1D] != EMPTY) {
     auto dest1D = to1D(destination);
     auto movedVersion = movedEquivalent(
                          static_cast<ZobristHasher::PieceIndex>(board[src1D]));
     auto stateBeforeMove = PastMove(*this, src1D, dest1D);

     // reset en passant piece to what it was before
     // regardless of the move, the right to en passant is gone
     for (auto const& [coord, piece] : pawnsBeforeEnPassant) {
       replace(coord, piece);
     }
     pawnsBeforeEnPassant.clear();

     if (isEnPassantRow(destination.row)) {
       auto left = Coordinates(destination.column-1, destination.row);
       auto right = Coordinates(destination.column+1, destination.row);
       std::vector<int> coords1D;
       if (areWithinLimits(left)) coords1D.push_back(to1D(left));
       if (areWithinLimits(right)) coords1D.push_back(to1D(right));

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
     movesHistory.push_back(std::move(stateBeforeMove));
   }
}

void ZobristHasher::removed(Coordinates const& coords) {
  auto coords1D = to1D(coords);
  if (board[coords1D] != EMPTY) {
    movesHistory.emplace_back(*this, coords1D, coords1D);
    remove(coords1D);
  }
}

int ZobristHasher::hash() {
  return currentHash;
}

void ZobristHasher::replacedWithPromotion(Coordinates const& source,
                                   PromotionOption prom, Colour colour) {
  PieceIndex replacement;
  switch (prom) {
  case PromotionOption::Queen:
    replacement = (colour == Colour::White) ?
      PieceIndex::WhiteQueen : PieceIndex::BlackQueen; break;
  case PromotionOption::Bishop:
    replacement = (colour == Colour::White) ? 
      PieceIndex::WhiteBishop : PieceIndex::BlackBishop; break;
  case PromotionOption::Knight:
    replacement = (colour == Colour::White) ?
      PieceIndex::WhiteKnight : PieceIndex::BlackKnight; break;
  case PromotionOption::Rook:
    replacement = (colour == Colour::White) ?
      PieceIndex::WhiteRook : PieceIndex::BlackRook; break;
  default:
    throw std::logic_error("Promotion not implemented correctly");
  }
  auto src1D = to1D(source);
  auto stateBeforeMove = PastMove(*this, src1D, src1D);
  replace(src1D, replacement);
  movesHistory.push_back(std::move(stateBeforeMove));
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

template <typename Predicate>
void ZobristHasher::initializePieces(std::vector<Coordinates> const& coords,
                                     PieceIndex piece,
                                     Predicate&& isNormalStartingCoord) {
  for (auto const& coord : coords) {
    if (!areWithinLimits(coord)) {
      throw std::invalid_argument("Coordinates go beyond the board limits");
    }

    auto coord1D = to1D(coord);
    if (board[coord1D] != EMPTY) {
      throw std::invalid_argument("Cannot initialize board with two or more"
                                  " pieces in the same coordinates");
    }

    if (!isNormalStartingCoord(coord)) {
     piece = movedEquivalent(piece);
    }
    board[coord1D] = static_cast<int>(piece);
  }
}

void ZobristHasher::standardInitBoard() {
  std::vector<Coordinates> whitePawns, blackPawns;
  for (int i = 0; i <= Board::MAX_COL_NUM; i++) {
    whitePawns.emplace_back(i, 1);
    blackPawns.emplace_back(i, Board::MAX_ROW_NUM - 1);
  }
  initializePieces(whitePawns, Rook::WHITE_STD_INIT, Knight::WHITE_STD_INIT,
    Bishop::WHITE_STD_INIT, {Queen::WHITE_STD_INIT}, King::WHITE_STD_INIT,
    blackPawns, Rook::BLACK_STD_INIT, Knight::BLACK_STD_INIT,
    Bishop::BLACK_STD_INIT, {Queen::BLACK_STD_INIT}, King::BLACK_STD_INIT);
}

int ZobristHasher::to1D(Coordinates const& coords) {
  if (!areWithinLimits(coords)) {
    throw std::out_of_range("Coordinates beyond hasher board limits");
  }
  return ((Board::MAX_ROW_NUM + 1) * coords.row) + coords.column;
}

bool ZobristHasher::areWithinLimits(Coordinates const& coords) {
  return (coords.column <= Board::MAX_COL_NUM && coords.column >= 0 &&
    coords.row <= Board::MAX_ROW_NUM && coords.row >= 0);
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
  return row == Board::MAX_ROW_NUM - 3 || row == 3;
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

void ZobristHasher::initializePieces(std::vector<Coordinates> const& whitePawns,
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
                                  Coordinates const& blackKing) {
  board.fill(EMPTY);

  initializePieces(whitePawns, PieceIndex::WhitePawn,
    [](Coordinates const& c) {
        return std::find(Pawn::WHITE_STD_INIT.begin(),
              Pawn::WHITE_STD_INIT.end(), c) != Pawn::WHITE_STD_INIT.end();
    });

  initializePieces(whiteRooks, PieceIndex::WhiteRook,
    [](Coordinates const& c) {
        return std::find(Rook::WHITE_STD_INIT.begin(),
              Rook::WHITE_STD_INIT.end(), c) != Rook::WHITE_STD_INIT.end();
    });

  // not all pieces have a moved equivalent in the hasher
  // return true for them as to avoid useless branching
  auto const returnTrue = [](Coordinates const&) { return true; };
  initializePieces(whiteKnights, PieceIndex::WhiteKnight, returnTrue);
  initializePieces(whiteBishops, PieceIndex::WhiteBishop, returnTrue);
  initializePieces(whiteQueens, PieceIndex::WhiteQueen, returnTrue);
  initializePieces(blackKnights, PieceIndex::BlackKnight, returnTrue);
  initializePieces(blackBishops, PieceIndex::BlackBishop, returnTrue);
  initializePieces(blackQueens, PieceIndex::BlackQueen, returnTrue);

  initializePieces({whiteKing}, PieceIndex::WhiteKing,
    [](Coordinates const& c) { return c == King::WHITE_STD_INIT; });
  
  initializePieces(blackPawns, PieceIndex::BlackPawn,
    [](Coordinates const& c) {
       return std::find(Pawn::BLACK_STD_INIT.begin(),
                   Pawn::BLACK_STD_INIT.end(), c) != Pawn::BLACK_STD_INIT.end();
    });
  
  initializePieces(blackRooks, PieceIndex::BlackRook,
    [](Coordinates const& c) {
       return std::find(Rook::BLACK_STD_INIT.begin(),
                   Rook::BLACK_STD_INIT.end(), c) != Rook::BLACK_STD_INIT.end();
    });
  
  initializePieces({blackKing}, PieceIndex::BlackKing,
    [](Coordinates const& c) { return c == King::BLACK_STD_INIT; });
}

void ZobristHasher::restorePreviousHash() {
  if (!movesHistory.empty()) {
    auto& lastMove = movesHistory.back();
    auto piece = static_cast<PieceIndex>(lastMove.sourceContent);
    replace(lastMove.source, piece);

    if (lastMove.destinationContent == EMPTY) {
      remove(lastMove.destination);
    } else {
      replace(lastMove.destination,
              static_cast<PieceIndex>(lastMove.destinationContent));
    }

    pawnsBeforeEnPassant = lastMove.pawnsBeforeEnPassant;
    for (auto const& [coord, pawn] : lastMove.pawnsBeforeEnPassant) {
      if (auto pawnEnPassant = getEnPassantPawn(pawn)) {
        replace(coord, *pawnEnPassant);
      }
    }
    currentHash = lastMove.hashBeforeMove;
    
    movesHistory.pop_back();
  }
}

ZobristHasher::~ZobristHasher() = default;

}
