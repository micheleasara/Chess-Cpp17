cmake_minimum_required(VERSION 3.22)

include(FetchContent)
FetchContent_Declare(
  googletest
  # GoogleTest 1.11.0
  URL https://github.com/google/googletest/archive/e2239ee6043f73722e7aa812a459f54a28552929.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(TestingLibs gtest_main gmock_main ChessCpp)

include(GoogleTest)
add_executable(BishopTest BishopTest.cpp)
target_link_libraries(BishopTest ${TestingLibs})
gtest_discover_tests(BishopTest)

include(GoogleTest)
add_executable(BoardTest BoardTest.cpp)
target_link_libraries(BoardTest ${TestingLibs})
gtest_discover_tests(BoardTest)

include(GoogleTest)
add_executable(KingTest KingTest.cpp)
target_link_libraries(KingTest ${TestingLibs})
gtest_discover_tests(KingTest)

include(GoogleTest)
add_executable(KnightTest KnightTest.cpp)
target_link_libraries(KnightTest ${TestingLibs})
gtest_discover_tests(KnightTest)

include(GoogleTest)
add_executable(PawnTest PawnTest.cpp)
target_link_libraries(PawnTest ${TestingLibs})
gtest_discover_tests(PawnTest)

include(GoogleTest)
add_executable(PieceTest PieceTest.cpp)
target_link_libraries(PieceTest ${TestingLibs})
gtest_discover_tests(PieceTest)

include(GoogleTest)
add_executable(QueenTest QueenTest.cpp)
target_link_libraries(QueenTest ${TestingLibs})
gtest_discover_tests(QueenTest)

include(GoogleTest)
add_executable(RookTest RookTest.cpp)
target_link_libraries(RookTest ${TestingLibs})
gtest_discover_tests(RookTest)

include(GoogleTest)
add_executable(ZobristHasherTest ZobristHasherTest.cpp)
target_link_libraries(ZobristHasherTest ${TestingLibs})
gtest_discover_tests(ZobristHasherTest)