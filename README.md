## What is this?
A chess engine written in C++ 17, together with a small driver program which lets you play chess on the console for explanatory purposes.  

It allows all traditional piece movements, including en passant, pawn promotion and castling. Every move can be undone, so as to restore the game state to what it was before the move occurred. Furthermore, 3-fold and 5-fold repetition, stalemate, the 50-moves rule, the 75-moves rule and draw for insufficient material are supported.

## How do I build it? What about testing?
I provided a makefile which will build the driver program for you with GCC. I also wrote several tests using GoogleTest 1.10 and Visual Studio Community 2019, which you can run by yourself as long as you have a proper IDE and build system.

## I want to use your chess engine on my chess application. What can I do?
You need to include _Board.hpp_ and rely on its _move_ overloads. You can either provide the source and destination as strings, or as numerical values. The result of a move can be determined by inspecting the returned object, for example to verify whether a piece was captured. Some special game states (such as the right to claim a draw or a pending pawn promotion) will need to be checked explicitly with the appropriate functions. Importantly, when the game finishes you need to reset or re-create the board in order to start a new session. I would suggest having a look at the (relatively short) driver program to see how a standard chess game may be implemented.

If, on the other hand, you are interested in generating a game starting in a non-standard position, I provided a constructor which allows you to specify a custom initial configuration. This would be the right choice if one is interested in studying or simulating mid or end game situations. Please refer to the documentation for the details. 

## Potential improvements
1) There is high coupling between the pieces and the board, mostly to allow for en passant via a visitor pattern.
2) The Board class contains the game state. This may or may not be better suited to a dedicated Game object.
