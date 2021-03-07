#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include <unordered_map>
#include <string>
#include <vector>
#include "Exceptions.hpp"
#include <memory>
#include "MoveResult.hpp"
#include <optional>
#include <ostream>
#include "Piece.hpp"
#include <string_view>
#include "Utils.hpp"
#include "Zobrist.hpp"

namespace Chess {
  class King; class Pawn; class Queen; class Knight; class Rook; class Bishop;

  /**
   Represents a chessboard. It is responsible for executing moves while
   containing the state of the game.
  */
  class Board {
  public:
    /// Defines the minimum column.
    static char constexpr MIN_COLUMN = 'A';
    /// Defines the minimum row.
    static char constexpr MIN_ROW = '1';
    /// Defines the maximum column.
    static char constexpr MAX_COLUMN = 'H';
    /// Defines the maximum row.
    static char constexpr MAX_ROW = '8';
    /// The maximum row number starting to count from 0.
    static int constexpr MAX_ROW_NUM = (int)(MAX_ROW - MIN_ROW);
    /// The maximum column number starting to count from 0.
    static int constexpr MAX_COL_NUM = (int)(MAX_COLUMN - MIN_COLUMN);
    /// Defines the starting positions of the white king.
    static auto constexpr WHITE_KING_INIT = Coordinates(4, 0);
    /// Defines the starting positions of the black king.
    static auto constexpr BLACK_KING_INIT = Coordinates(4, 7);
    /// Defines the number of squares the king travels to castle.
    static int constexpr CASTLE_DISTANCE = 2;
    /// Defines the horizontal printing space used for a square of the board.
    static int constexpr H_PRINT_SIZE = 15;

    /// Checks if the coordinates are within a chessboard.
    static bool areWithinLimits(Coordinates const& coord);
    /// Checks if the coordinates are in the same row.
    static bool areInSameRow(Coordinates const& coord1,
                             Coordinates const& coord2);
    /// Checks if the coordinates are in the same column.
    static bool areInSameColumn(Coordinates const& coord1,
                                Coordinates const& coord2);
    /// Checks if the coordinates are in the same diagonal.
    static bool areInSameDiagonal(Coordinates const& coord1,
                                  Coordinates const& coord2);
    /**
     Converts string coordinates into a pair of integers (eg "A2" to 0,1).
     Throws if the format is incorrect, or the coordinates are out of bounds.
    */
    static Coordinates stringToCoordinates(std::string_view coord);
    /// Converts numeric coordinates into string coordinates (eg 0,1 to "A2").
    static std::string coordinatesToString(Coordinates const& coord);

    /// Constructs a chessboard and places all pieces in their starting positions.
    Board();

    /// Resets the chessboard to its initial status.
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
     Moves the piece from the source to the destination provided,
     if the move is legitimate.
    */
    MoveResult move(Pawn& piece, Coordinates const& source,
                                 Coordinates const& destination);
    //! @copydoc Board::move(Pawn&,Coordinates const&,Coordinates const&)
    MoveResult move(PromotionPiece& piece, Coordinates const& source,
                                           Coordinates const& destination);
    //! @copydoc Board::move(Pawn&,Coordinates const&,Coordinates const&)
    MoveResult move(King& piece, Coordinates const& source,
                                 Coordinates const& destination);

    /**
     Retrieves the piece corresponding to the coordinates.
     Returns an empty optional if no piece is found at those coordinates.
    */
    OptionalRef<Piece> getPieceAtCoordinates(Coordinates const& coord) const;

    /// Determines if a pawn can do en passant from a source to a destination.
    bool isValidEnPassant(Pawn const& pawn, Coordinates const& source,
                                          Coordinates const& destination) const;

    /**
     Checks if there are no pieces from the source to the destination.
     The check is not inclusive of the start and end columns.
    */
    bool isColumnFree(Coordinates const& source, int limitRow) const;

    /**
     Checks if there are no pieces from the source to the destination.
     The check is not inclusive of the start and end rows.
    */
    bool isRowFree(Coordinates const& source, int limitCol) const;

    /**
     Checks if there are no pieces from the source to the destination.
     The check is not inclusive of the start and end positions.
    */
    bool isDiagonalFree(Coordinates const& source,
                        Coordinates const& destination) const;

    /// Returns true is a player needs to promote a piece. False otherwise.
    bool isPromotionPending() const;

    /**
     If a promotion is pending, the piece is replaced with the given promotion
     piece. Returns an object containing information about the board.
     Returns an empty optional when no promotion is pending.
    */
    std::optional<MoveResult> promote(PromotionOption piece);

    /// Returns the current player. White always starts.
    Piece::Colour currentPlayer() const;

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
    void initializePieces();
    Coordinates getPieceCoordinates(Piece const& piece) const;
    template <typename Callable>
    MoveResult move(Coordinates const& source, Coordinates const& destination,
                                                          Callable&& mover);
    void revertLastPieceMovement();
    std::optional<CastlingType> tryCastling(Coordinates const& source,
                                            Coordinates const& target);
    bool hasMovesLeft(Piece::Colour colour);
    bool isKingInCheck(Piece::Colour kingColour) const;
    bool isMoveSuicide(Coordinates sourceCoord, Coordinates targetCoord);
    void recordAndMove(Coordinates const& source,
                       Coordinates const& destination);
    void ensureGameNotOver();
    void ensurePlayerCanMovePiece(Piece const& piece);
    MoveResult::GameState checkGameState();
    void ensureNoPromotionNeeded();
    void togglePlayer();
    std::unique_ptr<PromotionPiece> buildPromotionPiece(PromotionOption piece);
    bool isMaterialSufficient() const;
    bool isPieceAtSource(Piece const& piece, Coordinates const& source) const;
    void ensurePieceIsAtSource(Piece const& piece,
                               Coordinates const& source) const;

    bool m_isGameOver = false;
    bool isWhiteTurn = true;
    std::optional<Coordinates> promotionSource;
    std::unordered_map<Coordinates, std::unique_ptr<Piece>,
                                                       CoordinatesHasher> board;
    std::unordered_map<Piece::Colour, King&> kings;
    ZobristHasher zobrist;
    std::unordered_map<int, size_t> boardHashCount;
    bool threeFoldRepetition = false;
    int countSincePawnMoveOrCapture = 0;
    std::unordered_set<std::reference_wrapper<Piece>,
                                          PieceRefHasher> insufficientMaterial;
    struct PastMove;
    std::vector<PastMove> movesHistory;
  };

}

#endif
