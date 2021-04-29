#include "Utils.hpp"
#include "Piece.hpp"

namespace Chess {

bool Coordinates::operator== (Coordinates const& other) const {
  return column == other.column && row == other.row;
}

bool Coordinates::operator!= (Coordinates const& other) const {
  return !operator==(other);
}

std::size_t PieceRefHasher::operator()(
                                std::reference_wrapper<Piece> const& p) const {
  // use address of pointed Piece as hash
  return reinterpret_cast<std::size_t>(&(p.get()));
}

}
