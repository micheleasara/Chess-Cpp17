#include "AbstractBoard.hpp"
#include "Utils.hpp"

namespace Chess {

bool AbstractBoard::areWithinLimits(Coordinates const& coord) {
  char row = static_cast<char>(coord.row) + MIN_ROW;
  char col = static_cast<char>(coord.column) + MIN_COLUMN;
  return (row >= MIN_ROW && row <= MAX_ROW &&
          col >= MIN_COLUMN && col <= MAX_COLUMN);
}

}