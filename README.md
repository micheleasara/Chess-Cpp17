## What is this?
A chess engine written in C++ 17, together with a small driver program which lets you play chess on the console for explanatory purposes.  

It allows all traditional piece movements, including en passant, pawn promotion and castling. Every move can be undone, so as to restore the game state to what it was before the move occurred. Furthermore, 3-fold and 5-fold repetition, stalemate, the 50-moves rule, the 75-moves rule and draw for insufficient material are supported.

## How do I build it? What about testing?
Ensure you have CMake 3.22 or above installed.

To build the driver program:
1) navigate to the _driver_ subdirectory;
2) create a folder that will contain the build (e.g. name it _build_);
3) navigate to the newly created folder;
4) enter ```cmake ..``` in your terminal (use ```cmake -DTESTS=OFF ..``` instead to avoid building the tests);
5) type ```cmake --build .```;
6) the executable _driver_ will be available in the build folder, whereas the library will be in _driver/lib_ and the tests in _driver/lib/tests_.

To run all the tests, simply navigate to their folder after building and type ```ctest```. Note that there is a dependency on GoogleTest, which will be downloaded during the build process.

To only build the library, repeat the steps above but:
1) navigate to the _ChessCpp_ subfolder instead of _driver_;
2) tests will be disabled by default, and you need to set the flag to ```ON``` to enable them;
3) remember you can use CMake's ```--config``` parameter if you wish to change the build mode to Release or similar.

## I want to use your chess engine on my chess application. What can I do?
Firstly build the library as described in the relative section. Then you can link it with your program.
You can also install the library by building it and then typing ```cmake --install .```.

Once the library is available, you need to include _Board.hpp_ and rely on its _move_ overloads. You can either provide the source and destination as strings, or as numerical values. The result of a move can be determined by inspecting the returned object, for example to verify whether a piece was captured. Some special game states (such as the right to claim a draw or a pending pawn promotion) will need to be checked explicitly with the appropriate functions. Importantly, when the game finishes you need to reset or re-create the board in order to start a new session. I would suggest having a look at the (relatively short) driver program to see how a standard chess game may be implemented.

If, on the other hand, you are interested in generating a game starting in a non-standard position, I provided a constructor which allows you to specify a custom initial configuration. This would be the right choice if one is interested in studying or simulating mid or end game situations. Please refer to the documentation for the details. 

## Potential improvements
1) The undo system would benefit from some refactoring. A cohesive undo-redo system may be a good idea.
2) The Board class contains the game state. This may or may not be better suited to a dedicated Game object.
3) Some of the tests concerning the pieces may benefit from mocking of the board.
4) The pieces have a reference to the AbstractBoard that contains them.
This is because the pieces have state that affects their behaviour, and thus must not be shared by different boards at the same time.
Instead of passing the board on construction, we could add an extra parameter to the methods that need the board.
On one hand, doing so would add an extra parameter that always takes the same value. On the other hand, we could more easily reuse the
pieces with a board that implements a different interface.