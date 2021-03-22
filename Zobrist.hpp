#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <unordered_set>
#include <vector>
#include "BoardHasher.hpp"

namespace Chess {

/// An implementation of Zobrist hashing for a chessboard.
class ZobristHasher: public BoardHasher {
public:
  /// Constructs a hasher for a board of the given size.
  ZobristHasher(size_t width, size_t height);

  //! @copydoc BoardHasher::pieceMoved(Coordinates const&,Coordinates const&)
  void pieceMoved(Coordinates const& source,
                  Coordinates const& destination) override;

  //! @copydoc BoardHasher::hash()
  int hash() override;

  //! @copydoc BoardHasher::restorePreviousHash()
  void restorePreviousHash() override;

  //! @copydoc BoardHasher::removed(Coordinates const&)
  void removed(Coordinates const& coords) override;

  //! @copydoc BoardHasher::replacedWithPromotion(Coordinates const&,PromotionOption,Piece::Colour)
  void replacedWithPromotion(Coordinates const& source,
                             PromotionOption prom,
                             Piece::Colour colour) override;

  //! @copydoc BoardHasher::reset()
  void reset() override;

  //! @copydoc BoardHasher::togglePlayer()
  void togglePlayer() override;

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
