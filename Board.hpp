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
    static int constexpr MAX_ROW_NUM = static_cast<int>(MAX_ROW - MIN_ROW);
    /// The maximum column number starting to count from 0.
    static int constexpr MAX_COL_NUM = static_cast<int>(MAX_COLUMN - MIN_COLUMN);

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

     When a piece is initialised in a non-standard location, it is considered
     to have moved. So, for instance, castling rights would not apply to a king
     initialised in A3. Furthermore, en passant rights are not given
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
     Performs move construction with a cost of O(N^2), where N is the total
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
     Retrieves the piece corresponding to the coordinates given.
     Returns an empty optional if no piece is found at those coordinates.
    */
    OptionalRef<Piece> getPieceAtCoordinates(Coordinates const& coord) const;

    /**
     Retrieves the coordinates corresponding to the piece given.
     Returns an empty optional if the piece is not on this board.
    */
    std::optional<Coordinates> getPieceCoordinates(Piece const& piece) const;

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
    void revertLastPieceMovement();
    std::optional<CastlingType> tryCastling(Coordinates const& source,
                                            Coordinates const& target);
    bool hasMovesLeft(Colour colour);
    bool isKingInCheck(Colour kingColour) const;
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

    template <typename Chessman, typename Predicate>
    void initializePieces(std::vector<Coordinates> const& coords,
                          Colour colour,
                          Predicate&& isNormalStartingColumn);

    template <typename Chessman, typename Predicate, typename Finisher>
    void initializePieces(std::vector<Coordinates> const& coords,
                          Colour colour,
                          Predicate&& isNormalStartingColumn,
                          Finisher&& finalActions);

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
    bool isWhiteTurn = true;
    std::optional<Coordinates> promotionSource;
    std::unordered_map<Coordinates, std::unique_ptr<Piece>,
                                                       CoordinatesHasher> board;
    std::unordered_map<Colour, King&> kings;
    std::unique_ptr<BoardHasher> hasher;
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
