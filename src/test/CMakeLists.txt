cmake_minimum_required(VERSION 3.20)
project(Chess)

find_package(folly CONFIG REQUIRED)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED)

set(CMAKE_CXX_STANDARD 23)
file(GLOB SOURCES "*.cpp")
include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(
        all_tests ../main/board/Board.cpp ../main/board/Move.cpp ../main/board/Piece.cpp
        ../main/board/zobrist_hash_generator.cpp ../main/ai/MoveGenerator.cpp board/BoardTest.cpp
        ai/MoveGeneratorTest.cpp board/MoveTest.cpp board/zobrist_hash_generator_test.cpp
        ../main/board/move_processor.h ../main/board/move_processor.cpp
        ../main/ai/square_value_tables.h ../main/ai/square_value_tables.cpp
        ../main/ai/analysis_info.h
)
target_link_libraries(all_tests gtest_main Folly::folly ${BOOST_LIBRARIES})
