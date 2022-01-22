cmake_minimum_required(VERSION 3.20)
project(Chess)

set(CMAKE_CXX_STANDARD 23)
file(GLOB SOURCES "*.cpp")
include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(tests ../main/Board.cpp ../main/Move.cpp ../main/Piece.cpp ../main/zobrist_hash_generator.cpp BoardTest.cpp MoveGeneratorTest.cpp MoveTest.cpp zobrist_hash_generator_test.cpp)
target_link_libraries(tests gtest_main)
