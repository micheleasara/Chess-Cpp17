## What is this?
A chess engine written in C++ 17, together with a small driver program which lets you play chess on the console for explainatory purposes.  
  
It allows all traditional piece movements, including en passant, pawn promotion and castling. Furhermore, 3-fold and 5-fold repetition, stalemate, the 50-moves rule, the 75-moves rule and draw for insufficient material are all supported.

## How do I build it? What about testing?
I provided a makefile which will build the driver program for you. I also wrote several unit tests using googletest and Visual Studio Community 2019, which you can run by yourself as long as you have a proper IDE and build system.

## I want to use your chess engine on my chess application. What can I do?
You need to include _Board.hpp_ and rely on its _move_ overloards. Feel free to add others as you see fit. Alternatively, you can use _Piece::move_.  
I would suggest having a look at the (relatively short) driver program, but, regardless, all public functions are documented.

## Known issues and areas of improvement
There is high coupling between the pieces and the board, mostly to allow for en passant via a visitor pattern. Also, there is no dependency injection in the board, which makes it a bit trickier to test.
