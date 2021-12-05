cmake_minimum_required(VERSION 3.20)
project(Chess)

set(CMAKE_CXX_STANDARD 23)

file(GLOB SOURCES "*.cpp")
add_executable(Chess Board.cpp Move.cpp Piece.cpp main.cpp)
include(FetchContent)
FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(tests Board.cpp Move.cpp Piece.cpp BoardTest.cpp MoveGeneratorTest.cpp MoveTest.cpp)
target_link_libraries(tests gtest_main)
add_test(NAME example_test COMMAND example)
