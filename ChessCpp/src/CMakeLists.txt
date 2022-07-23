cmake_minimum_required(VERSION 3.22)

set(headers Bishop.hpp Board.hpp BoardHasher.hpp Exceptions.hpp
            King.hpp Knight.hpp MoveResult.hpp Pawn.hpp Piece.cpp
            Queen.hpp Rook.hpp Utils.hpp Zobrist.hpp)
add_library(ChessCpp ${headers} Bishop.cpp Board.cpp Exceptions.cpp King.cpp
                                Knight.cpp MoveResult.cpp Pawn.cpp Piece.cpp
                                Queen.cpp Rook.cpp Utils.cpp Zobrist.cpp)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  if(MSVC)
    target_compile_options(ChessCpp PRIVATE /W4)
  endif()

  if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(ChessCpp PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endif(CMAKE_BUILD_TYPE MATCHES Debug)

install(TARGETS ChessCpp DESTINATION "lib/")
install(FILES ${headers} DESTINATION "include/ChessCpp")