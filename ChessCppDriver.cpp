#include "Board.hpp"
#include <iostream>
#include <ostream>
#include <algorithm>
#include <cctype>

using Chess::Board;
using Chess::PromotionOption;
using Chess::Piece;
using Chess::MoveResult;
using Chess::CastlingType;

std::optional<PromotionOption> strToPromotionPiece(std::string const& piece);
void printIfSpecialState(MoveResult::GameState state,
                        std::string const& opponent);
void printCastlingMessage(CastlingType type, std::string const& player);
void promptForDraw(Board& cb);
void promptToReset(Board& cb);

int main() {
    using namespace std;
    Board cb;

    while (!cb.isGameOver()) {
      try {
        cout << cb;
        string source, dest;
        string currentPlayer, opponent;
        if (cb.currentPlayer() == Piece::Colour::White) {
          currentPlayer = "White";
          opponent = "Black";
        } else {
          currentPlayer = "Black";
          opponent = "White";
        }
        cout << "It's " << currentPlayer << "'s turn.\n";

        cout << "Please input source coordinates (e.g. A2)\n";
        getline(cin, source);
        cout << "Please input destination coordinates (e.g. A3)\n";
        getline(cin, dest);

        auto result = cb.move(source, dest);
        if (auto castlingType = result.castlingType()) {
          printCastlingMessage(*castlingType, currentPlayer);
        } else {
          auto destCoord = Board::stringToCoordinates(dest);
          if (auto movedPiece = cb.getPieceAtCoordinates(destCoord)) {
            cout << *movedPiece << " moves from " << source << " to " << dest;
            if (auto eatenName = result.capturedPieceName()) {
              cout << " taking " << opponent << "'s " << *eatenName;
            }
            cout << "\n";
          }
        }

        while (cb.isPromotionPending()) {
          cout << cb;
          cout << "\nPlease enter a valid piece for pawn promotion.\n";
          cout << "Possible options: knight, rook, bishop, and queen.\n";
          string promotionInput;
          getline(std::cin, promotionInput);

          if (auto piece = strToPromotionPiece(promotionInput)) {
            result = cb.promote(*piece);
          }
        }

        printIfSpecialState(result.gameState(), opponent);

        if (cb.drawCanBeClaimed()) {
          promptForDraw(cb);
        }

        if (cb.isGameOver()) {
          promptToReset(cb);
        }
        
      } catch (exception const& e) {
        cout << e.what() << "\n";
      }
    }
}

void printCastlingMessage(CastlingType type, std::string const& player) {
  if (type == CastlingType::KingSide) {
    std::cout << player << " castles king side.\n";
  } else {
    std::cout << player << " castles queen side.\n";
  }
}

std::optional<PromotionOption> strToPromotionPiece(std::string const& piece) {
  std::string lowercased;
  std::transform(piece.begin(), piece.end(), std::back_inserter(lowercased),
    [](unsigned char c) { return std::tolower(c); });
  if (lowercased == "knight") {
    return PromotionOption::Knight;
  }
  else if (lowercased == "rook") {
    return PromotionOption::Rook;
  }
  else if (lowercased == "bishop") {
    return PromotionOption::Bishop;
  }
  else if (lowercased == "queen") {
    return PromotionOption::Queen;
  }
  return std::nullopt;
}

void printIfSpecialState(MoveResult::GameState state,
  std::string const& opponent) {
  using namespace std;
  switch (state) {
  case MoveResult::GameState::OPPONENT_IN_CHECK:
    cout << opponent << " is in check.\n"; break;
  case MoveResult::GameState::OPPONENT_IN_CHECKMATE:
    cout << opponent << " is in checkmate.\n"; break;
  case MoveResult::GameState::INSUFFICIENT_MATERIAL_DRAW:
    cout << "Game ends as a draw due to insufficient material.\n"; break;
  case MoveResult::GameState::FIVEFOLD_REPETITION_DRAW:
    cout << "Game ends as a draw due to five-fold repetition.\n"; break;
  case MoveResult::GameState::STALEMATE:
    cout << "Game ends as a draw due to stalemate.\n"; break;
  case MoveResult::GameState::SEVENTYFIVE_MOVES_DRAW:
    cout << "Game ends as a draw due to the 75 moves rule.\n"; break;
  default:
    break;
  }
}

void promptForDraw(Board& cb) {
  std::cout << "A draw can be claimed. Enter 'y' to claim, or enter ";
  std::cout << "anything else to continue with the game:\n";
  std::string input;
  std::getline(std::cin, input);
  if (input == "y") {
    cb.claimDraw();
    std::cout << "You claimed a draw.\n";
  }
}

void promptToReset(Board& cb) {
  std::cout << "\nGame over!\n";
  std::cout << "Enter 'r' to reset, or anything else to exit.\n";
  std::string input;
  std::getline(std::cin, input);
  if (input == "r") {
    cb.reset();
  }
}