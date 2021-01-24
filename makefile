OBJ = Utils.o MoveResult.o ChessCppDriver.o Board.o Piece.o \
Rook.o Bishop.o Queen.o Knight.o King.o Pawn.o Zobrist.o Exceptions.o
EXE = chess
CXX = g++
CXXFLAGS = -Wall -g -Wextra -MMD -std=c++17

$(EXE): $(OBJ)
	$(CXX) $^ -o $@

%.o: %.cpp makefile
	$(CXX) $(CXXFLAGS) -c $<

-include $(OBJ:.o=.d)

clean:
	rm -f $(OBJ) $(EXE) $(OBJ:.o=.d)
.PHONY: clean
