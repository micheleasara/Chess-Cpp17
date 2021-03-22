#ifndef BOARD_HASHER_H
#define BOARD_HASHER_H

#include <optional>
#include "Piece.hpp"

namespace Chess {

/// An object capable of hashing a chessboard configuration.
class BoardHasher {
public:
  /**
   Updates the hash by considering the piece at the source to have moved
   to the destination provided.
  */
  virtual void pieceMoved(Coordinates const& source,
                          Coordinates const& destination) = 0;

  /// Returns the most recent hash.
  virtual int hash() = 0;

  /**
   Restores the hasher to the state before the last change.
   Does nothing if called with no change has occurred yet.
  */
  virtual void restorePreviousHash() = 0;

  /**
   Updates the hash by considering the piece at the specified coordinates to
   have been removed.
   The hash does not change in case of deletion of an empty square.
  */
  virtual void removed(Coordinates const& coords) = 0;

  /**
   Updates the hash by assuming the source given to be replaced with a promotion
   piece.
  */
  virtual void replacedWithPromotion(Coordinates const& source,
                            PromotionOption prom, Piece::Colour colour) = 0;

  /// Resets the hasher to an initial configuration.
  virtual void reset() = 0;

  /// Changes the hash by toggling the current player. White always starts.
  virtual void togglePlayer() = 0;

  virtual ~BoardHasher() = default;
};

}

#endif // BOARD_HASHER_H