#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <unordered_set>
#include <array>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <optional>
#include <vector>
#include "Piece.hpp"

namespace Chess {

/// An object capable of hashing a chessboard configuration.
class ZobristHasher {
public:
  /// Constructs a hasher for a board of the given size.
  ZobristHasher(size_t width, size_t height);

  /**
   Updates the hash by considering the piece at the source to have moved
   to the destination provided.
   */
  void pieceMoved(Coordinates const& source, Coordinates const& destination);

  /// Returns the most recent hash.
  int hash();

  /**
   Restores the hasher to the state before the last change.
   Does nothing if called with no change has occurred yet.
  */
  void restorePreviousHash();

  /**
   Updates the hash by considering the piece at the specified coordinates to
   have been removed.
   The hash does not change in case of deletion of an empty square.
  */
  void removed(Coordinates const& coords);

  /**
   Updates the hash by assuming the source given to be replaced with a promotion
   piece.
  */
  void replacedWithPromotion(Coordinates const& source,
                            PromotionOption prom, Piece::Colour colour);

  /// Resets the hasher to an initial configuration.
  void reset();

  /// Changes the hash by toggling the current player. White always starts.
  void togglePlayer();

  virtual ~ZobristHasher();

private:
  static size_t constexpr PIECE_INDEXES_COUNT = 20;
  static int constexpr EMPTY = -1;
  enum class PieceIndex;
  struct PastMove;

  void initializeTableAndWhitePlayer();
  void fillInitialBoard();
  int computeHashFromBoard();
  int to1D(Coordinates const& coords);
  bool areWithinLimits(Coordinates const& coords);
  bool isEnPassantRow(int row);
  void replace(int coord1D, PieceIndex replacement);
  void remove(int coord1D);

  PieceIndex constexpr movedEquivalent(PieceIndex idx);
  std::optional<PieceIndex> getEnemyMovedPawn(PieceIndex pawn);
  std::optional<PieceIndex> getEnPassantPawn(PieceIndex pawn);

  size_t CHESSBOARD_AREA;
  int MAX_ROW_NUM, MAX_COL_NUM;
  std::vector<std::vector<int>> table;
  std::vector<int> board;
  int currentHash = 0;
  int whitePlayerHash;
  std::unordered_map<int, PieceIndex> pawnsBeforeEnPassant;
  std::vector<PastMove> movesHistory;
};

}

#endif // ZOBRIST_H
