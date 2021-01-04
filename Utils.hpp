#ifndef CHESS_UTILS
#define CHESS_UTILS

#include <utility>
#include <optional>
#include <functional>

namespace Chess {
class Piece;

/// An optional that can contain a reference type.
template <typename T>
using OptionalRef = std::optional<std::reference_wrapper<T>>;

/// Represents a pair of coordinates.
struct Coordinates {
  int column = 0;
  int row = 0;

  constexpr Coordinates() = default;
  constexpr Coordinates(int column, int row): column(column), row(row) {}

  /// Returns true if column and row are identical, false otherwise.
  bool operator== (Coordinates const& other) const;
  /// Returns true if column or row are different, false otherwise.
  bool operator!= (Coordinates const& other) const;
};

/// Performs hashing of coordinates.
struct CoordinatesHasher {
  std::size_t operator()(Coordinates const& coord) const;
};

/// Performs hashing of a reference wrapper containing a Piece.
struct PieceRefHasher {
  std::size_t operator()(std::reference_wrapper<Piece> const& p) const;
};

/// Represents a piece another can be promoted into.
enum class PromotionOption {
  Knight, Bishop, Rook, Queen
};

/// Represents a type of castling.
enum class CastlingType {
  KingSide,
  QueenSide,
};

}

namespace std {

// allow comparisons of std::reference_wrapper<Chess::Piece>, making
// creation of unordered_map of Piece reference wrappers possible
template<>
struct equal_to<std::reference_wrapper<Chess::Piece>> {
  bool operator()(std::reference_wrapper<Chess::Piece> const& lhs,
                  std::reference_wrapper<Chess::Piece> const& rhs) const {
    return &(lhs.get()) == &(rhs.get());
  }
};

}
#endif // CHESS_UTILS