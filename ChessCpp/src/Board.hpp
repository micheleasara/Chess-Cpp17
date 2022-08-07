#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "AbstractBoard.hpp"
#include <array>
#include "BoardHasher.hpp"
#include "Exceptions.hpp"
#include <memory>
#include "MoveResult.hpp"
#include <optional>
#include <ostream>
#include "Piece.hpp"
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include "Utils.hpp"
#include <vector>

namespace Chess {

class King; class Pawn; 

/**
  Represents a chessboard. It is responsible for executing moves while
  containing the state of the game.
*/
class Board final: public AbstractBoard {
public:
  /**
    Converts string coordinates into a pair of integers (eg "A2" to 0,1).
    Throws if the format is incorrect, or the coordinates are out of bounds.
  */
  static Coordinates stringToCoordinates(std::string_view coord);
  /// Converts numeric coordinates into string coordinates (eg 0,1 to "A2").
  static std::string coordinatesToString(Coordinates const& coord);

  /**
    Places all pieces in their standard starting positions.
    Defaults to Zobrist hashing for the 3-fold and 5-fold repetition rules.
  */
  Board();

  /**
    Places all pieces in their standard starting positions.
    Uses the hasher provided for the 3-fold and 5-fold repetition rules.
  */
  Board(std::unique_ptr<BoardHasher> hasher);

  /*
    Places the pieces on the board following a custom configuration.

    When a piece is initialised in a non-standard location, it is treated as
    if it had moved there. So, for instance, castling rights would not apply
    to a king initialised in A3. Furthermore, en passant rights are not given
    upon initialisation, meaning you cannot directly initialise two pawns for
    en passant and execute it the next turn.
    Finally, the board defaults to Zobrist hashing for the 3-fold and 5-fold
    repetition rules.

    Throws in case of:
    1) invalid coordinates;
    2) multiple pieces sharing the same coordinates;
    3) multiple promotions (e.g. two white pawns in the last row).
  */
  Board(std::vector<Coordinates> const& whitePawns,
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

  /**
    Performs move construction with a cost of O(N), where N is the total
    number of pieces that are and were on the board during this game.
  */
  Board(Board&& other) noexcept;

  /**
    Performs move assignment with a cost of O(N), where N is the total number
    of pieces that are and were on the board during this game.
  */
  Board& operator=(Board&& other) noexcept;

  /// Resets the chessboard to its standard, initial configuration.
  void reset();

  /**
    Performs a move from a source to a destination and alternates between
    players according to the rules of chess. Coordinates are given column first
    and row second, from MIN_COLUMN to MAX_COLUMN and MIN_ROW to MAX_ROW,
    respectively. So, for instance, "C2" translates to 3rd column, 2nd row.

    Returns an object containing information regarding the move executed.
    In case of invalid move, an InvalidMove exception is thrown.
  */
  MoveResult move(std::string_view src, std::string_view destination);

  /**
    Performs a move from a source to a destination and alternates between
    players according to the rules of chess.

    Returns an object containing information regarding the move executed.
    In case of invalid move, an InvalidMove exception is thrown.
  */
  MoveResult move(Coordinates const& src, Coordinates const& dest);

  /**
    Retrieves the piece corresponding to the coordinates given.
    Returns a nullptr if no piece is found at those coordinates.
    The pointer returned is non-owning.
  */
  Piece const* at(Coordinates const& coord) const override;

  /**
    Retrieves the coordinates corresponding to the piece given.
    Returns an empty optional if the piece is not on this board.
  */
  std::optional<Coordinates> getPieceCoordinates(Piece const& piece) const;

  /// Determines if a pawn can do en passant from a source to a destination.
  bool isValidEnPassant(Pawn const& pawn, Coordinates const& source,
                                Coordinates const& destination) const override;

  /**
    Checks if there are no pieces from the source to the destination.
    The check is not inclusive of the start and end columns.
  */
  bool isFreeColumn(Coordinates const& source, int limitRow) const override;

  /**
    Checks if there are no pieces from the source to the destination.
    The check is not inclusive of the start and end rows.
  */
  bool isFreeRow(Coordinates const& source, int limitCol) const override;

  /**
    Checks if there are no pieces from the source to the destination.
    The check is not inclusive of the start and end positions.
  */
  bool isFreeDiagonal(Coordinates const& source,
                      Coordinates const& destination) const override;

  /// Returns true if a player needs to promote a piece, false otherwise.
  bool promotionPending() const;

  /**
    If a promotion is pending, the piece is replaced with the given promotion
    piece. Returns an object containing information about the board.
    Returns an empty optional when no promotion is pending.
  */
  std::optional<MoveResult> promote(PromotionOption piece);

  /// Returns the current player. White always starts.
  Colour currentPlayer() const;

  /// Returns true if the game reached its conclusion, false otherwise.
  bool isGameOver() const;

  /// Prints the board to the output stream provided.
  friend std::ostream& operator<<(std::ostream& out, Board const& board);

  /// Returns true if the given player can claim a draw, false otherwise.
  bool drawCanBeClaimed() const;

  /// Ends the game in a draw if it can be claimed, does nothing otherwise.
  void claimDraw();

  /**
    Restores the board to the state before the last move.
    Does nothing if called with no recorded moves.
    Castling and promotion are considered one move.
  */
  void undoLastMove();

  virtual ~Board();

private:
  void initializePiecesInStandardPos();

  template <typename Callable>
  MoveResult move(Coordinates const& source, Coordinates const& destination,
                                                        Callable&& mover);
  MoveResult move(Pawn& piece, Coordinates const& source,
                                Coordinates const& destination) override;
  MoveResult move(PromotionPiece& piece, Coordinates const& source,
                                       Coordinates const& destination) override;
  MoveResult move(King& piece, Coordinates const& source,
                                Coordinates const& destination) override;

  void revertLastPieceMovement();
  std::optional<CastlingType> tryCastling(Coordinates const& source,
                                          Coordinates const& target);
  bool hasMovesLeft(Colour colour);
  bool pieceHasMovesLeft(Coordinates const& srcCoord);
  bool isInCheck(Colour kingColour) const;
  bool isSuicide(Coordinates const& sourceCoord, Coordinates const& targetCoord);
  void recordAndMove(Coordinates const& source,
                      Coordinates const& destination);
  void ensureGameNotOver();
  void ensurePlayerCanMovePiece(Piece const& piece);
  MoveResult::GameState checkGameState();
  void ensureNoPromotionNeeded();
  void togglePlayer();
  std::unique_ptr<PromotionPiece> buildPromotionPiece(PromotionOption piece);
  bool sufficientMaterial() const;
  void ensurePieceIsAtSource(Piece const& piece,
                              Coordinates const& source) const;

  template <typename Chessman, typename Predicate>
  void initializePieces(std::vector<Coordinates> const& coords,
                        Colour colour,
                        Predicate&& isStandardStartingPos);

  template <typename Chessman, typename Predicate, typename Callable>
  void initializePieces(std::vector<Coordinates> const& coords,
                        Colour colour,
                        Predicate&& isStandardStartingPos,
                        Callable&& finalActions);

  void initializePawns(std::vector<Coordinates> const& coords,
                        Colour colour);
  void initializeRooks(std::vector<Coordinates> const& coords,
                        Colour colour);
  void initializeKnights(std::vector<Coordinates> const& coords,
                        Colour colour);
  void initializeBishops(std::vector<Coordinates> const& coords,
                        Colour colour);
  void initializeQueens(std::vector<Coordinates> const& coords,
                        Colour colour);
  void initializeKing(Coordinates const& coords, Colour colour);

  bool m_isGameOver = false;
  bool m_isWhiteTurn = true;
  std::optional<Coordinates> m_promotionSource;
  std::array<std::array<std::unique_ptr<Piece>, MAX_ROW_NUM+1>,
                                                MAX_COL_NUM+1> m_board;
  std::unordered_map<Colour, King&> m_kings;
  std::unique_ptr<BoardHasher> m_hasher;
  std::unordered_map<int, size_t> m_boardHashCount;
  bool m_threeFoldRepetition = false;
  int m_countSincePawnMoveOrCapture = 0;
  std::unordered_set<std::reference_wrapper<Piece>,
                                   PieceRefHasher> m_insufficientMaterial;
  struct PastMove;
  std::vector<PastMove> m_movesHistory;
};

}

#endif
