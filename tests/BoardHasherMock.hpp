#include "pch.h"
#include "BoardHasher.hpp"

using Chess::Coordinates;

class BoardHasherMock: public Chess::BoardHasher {
public:
    MOCK_METHOD(void, pieceMoved, (Coordinates const& source,
                      Coordinates const& destination), (override));
    MOCK_METHOD(int, hash, (), (override));
    MOCK_METHOD(void, restorePreviousHash, (), (override));
    MOCK_METHOD(void, removed, (Coordinates const& coords), (override));
    MOCK_METHOD(void, replacedWithPromotion, (Coordinates const& source,
                                    Chess::PromotionOption prom,
                                    Chess::Colour colour), (override));
    MOCK_METHOD(void, reset, (), (override));
    MOCK_METHOD(void, togglePlayer, (), (override));
};