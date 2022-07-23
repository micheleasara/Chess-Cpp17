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
using Chess::Colour;

/// Prints an explanation of how to undo a move to the output stream.
void printUndoInstruction();

/**
 Requests to input a promotion type and returns the result of the promotion.
 Returns an empty optional if the user decided to undo the last move.
 The user will be prompted until a valid input is entered.
*/
std::optional<MoveResult> promptForPromotion(Board& cb);

/// Returns the corresponding promotion option, or an empty optional if invalid.
std::optional<PromotionOption> strToPromotionPiece(std::string_view piece);

/// Prints a message to the output stream in case of special state (eg check).
void printIfSpecialState(MoveResult::GameState state,
                        std::string_view opponent);

/// Prints a message to the output stream describing which castling occurred.
void printCastlingMessage(CastlingType type, std::string_view player);

/// Asks the user if they wish to claim a draw, and does the claim if so.
void promptForDraw(Board& cb);

/// Asks the user if they wish to reset the game, and resets the game if so.
void promptToReset(Board& cb);

std::string const UNDO_STR = "undo";

int main() {
    using namespace std;
    Board cb;

    while (!cb.isGameOver()) {
      try {
        cout << cb;
        string source, dest;
        string currentPlayer, opponent;
        if (cb.currentPlayer() == Colour::White) {
          currentPlayer = "White";
          opponent = "Black";
        } else {
          currentPlayer = "Black";
          opponent = "White";
        }
        cout << "It's " << currentPlayer << "'s turn.\n";

        cout << "Please input source coordinates (e.g. A2)\n";
        printUndoInstruction();
        getline(cin, source);
        if (source == UNDO_STR) {
          cb.undoLastMove();
          continue;
        }

        cout << "Please input destination coordinates (e.g. A3)\n";
        printUndoInstruction();
        getline(cin, dest);
        if (dest == UNDO_STR) {
          cb.undoLastMove();
          continue;
        }

        auto result = cb.move(source, dest);
        if (auto castlingType = result.castlingType()) {
          printCastlingMessage(*castlingType, currentPlayer);
        } else {
          auto destCoord = Board::stringToCoordinates(dest);
          if (auto movedPiece = cb.at(destCoord)) {
            cout << *movedPiece << " moves from " << source << " to " << dest;
            if (auto capturedName = result.capturedPieceName()) {
              cout << " taking " << opponent << "'s " << *capturedName;
            }
            cout << "\n";
          }
        }

        if (cb.promotionPending()) {
          if (auto promotionMoveResult = promptForPromotion(cb)) {
            result = *promotionMoveResult;
          } else {
            cb.undoLastMove();
            continue;
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

void printUndoInstruction() {
  std::cout << "Enter '" << UNDO_STR << "' to undo the last move.\n";
}

std::optional<MoveResult> promptForPromotion(Board& cb) {
  std::optional<MoveResult> result = std::nullopt;
  while (cb.promotionPending()) {
    std::cout << cb;
    std::cout << "\nPlease enter a valid piece for pawn promotion.\n";
    std::cout << "Possible options: knight, rook, bishop, and queen.\n";
    printUndoInstruction();

    std::string promotionInput;
    std::getline(std::cin, promotionInput);
    if (promotionInput == UNDO_STR) {
      break;
    } else if (auto piece = strToPromotionPiece(promotionInput)) {
      auto resultOpt = cb.promote(*piece);
      result = *resultOpt;
    }
  }
  return result;
}

void printCastlingMessage(CastlingType type, std::string_view player) {
  if (type == CastlingType::KingSide) {
    std::cout << player << " castles king side.\n";
  } else {
    std::cout << player << " castles queen side.\n";
  }
}

std::optional<PromotionOption> strToPromotionPiece(std::string_view piece) {
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
                         std::string_view opponent) {
  switch (state) {
  case MoveResult::GameState::OPPONENT_IN_CHECK:
    std::cout << opponent << " is in check.\n"; break;
  case MoveResult::GameState::OPPONENT_IN_CHECKMATE:
    std::cout << opponent << " is in checkmate.\n"; break;
  case MoveResult::GameState::INSUFFICIENT_MATERIAL_DRAW:
    std::cout << "Game ends as a draw due to insufficient material.\n"; break;
  case MoveResult::GameState::FIVEFOLD_REPETITION_DRAW:
    std::cout << "Game ends as a draw due to five-fold repetition.\n"; break;
  case MoveResult::GameState::STALEMATE:
    std::cout << "Game ends as a draw due to stalemate.\n"; break;
  case MoveResult::GameState::SEVENTYFIVE_MOVES_DRAW:
    std::cout << "Game ends as a draw due to the 75 moves rule.\n"; break;
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