#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "BoardHasher.hpp"
#include <unordered_set>
#include <vector>
#include <array>
#include "Board.hpp"

namespace Chess {

/// An implementation of Zobrist hashing for a chessboard.
class ZobristHasher final: public BoardHasher {
public:
  /*
   Constructs a hasher for a chessboard, considering all pieces to be in their
   standard starting positions.
  */
  ZobristHasher();

  /*
   Constructs a hasher for a chessboard, and considers the pieces to be on the
   board following a custom configuration.

   When a piece is initialised in a non-standard location, it is treated as if
   it had moved there. So, for instance, castling rights would not apply to a 
   king initialised in A3. Furthermore, en passant rights are not given
   upon initialisation.

   Throws in case of:
   1) invalid coordinates;
   2) multiple pieces sharing the same coordinates;
  */
  ZobristHasher(std::vector<Coordinates> const& whitePawns,
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
      Coordinates const& blackKing);

  //! @copydoc BoardHasher::pieceMoved(Coordinates const&,Coordinates const&)
  void pieceMoved(Coordinates const& source,
                  Coordinates const& destination) override;

  //! @copydoc BoardHasher::hash()
  int hash() override;

  //! @copydoc BoardHasher::restorePreviousHash()
  void restorePreviousHash() override;

  //! @copydoc BoardHasher::removed(Coordinates const&)
  void removed(Coordinates const& coords) override;

  //! @copydoc BoardHasher::replacedWithPromotion(Coordinates const&,PromotionOption,Colour)
  void replacedWithPromotion(Coordinates const& source,
                             PromotionOption prom,
                             Colour colour) override;

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
  template <typename Predicate>
  void initializePieces(std::vector<Coordinates> const& coords, PieceIndex piece,
                        Predicate&& isNormalStartingCoord);
  void standardInitBoard();
  int computeHashFromBoard();
  int to1D(Coordinates const& coords);
  bool areWithinLimits(Coordinates const& coords);
  bool isEnPassantRow(int row);
  void replace(int coord1D, PieceIndex replacement);
  void remove(int coord1D);

  PieceIndex constexpr movedEquivalent(PieceIndex idx);
  std::optional<PieceIndex> getEnemyMovedPawn(PieceIndex pawn);
  std::optional<PieceIndex> getEnPassantPawn(PieceIndex pawn);

  void initializePieces(std::vector<Coordinates> const& whitePawns,
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
      Coordinates const& blackKing);

  std::array<std::array<int, PIECE_INDEXES_COUNT>, Board::AREA> table;
  std::array<int, Board::AREA> board;
  int currentHash = 0;
  int whitePlayerHash;
  std::unordered_map<int, PieceIndex> pawnsBeforeEnPassant;
  std::vector<PastMove> movesHistory;
};

}

#endif // ZOBRIST_H
