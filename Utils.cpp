#include "Utils.hpp"
#include "Piece.hpp"
#include <iostream>

namespace Chess {

bool Coordinates::operator== (Coordinates const& other) const {
  return column == other.column && row == other.row;
}

bool Coordinates::operator!= (Coordinates const& other) const {
  return !operator==(other);
}

std::size_t hashCombine(std::size_t lhs, std::size_t rhs) {
  lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
  return lhs;
}

std::size_t CoordinatesHasher::operator()(Coordinates const& p) const {
  auto hash1 = std::hash<int>{}(p.column);
  auto hash2 = std::hash<int>{}(p.row);
  return hashCombine(hash1, hash2);
}

std::size_t PieceRefHasher::operator()(
                                std::reference_wrapper<Piece> const& p) const {
  // use address of pointed Piece as hash
  return reinterpret_cast<std::size_t>(&(p.get()));
}

}