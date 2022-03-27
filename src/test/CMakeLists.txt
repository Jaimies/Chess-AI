cmake_minimum_required(VERSION 3.20)
project(Chess)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED)
find_package(TBB CONFIG REQUIRED)

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
        all_tests ../main/board/board.cpp ../main/board/move.cpp ../main/board/piece.cpp
        ../main/board/zobrist_hash_generator.cpp ../main/ai/move_generator.cpp board/board_test.cpp
        ai/move_generator_test.cpp board/move_test.cpp board/zobrist_hash_generator_test.cpp util/string_util_test.cpp
        ../main/board/move_processor.h ../main/board/move_processor.cpp
        ../main/ai/square_value_tables.h ../main/ai/square_value_tables.cpp
        ../main/ai/analysis_info.h ../main/ai/ai_settings.h
        ../main/ai/move_evaluation_data.h
        ../main/util/vector_util.h ../main/util/thread_util.h
        ../main/ai/constants.h ../main/ai/constants.cpp
        ../main/ai/deep_evaluation_strategy.h ../main/ai/deep_evaluation_strategy.cpp
        ../main/ai/move_sorting.h ../main/ai/move_sorting.cpp
        ../main/ai/transpositions.cpp ../main/ai/transpositions.h
        ../main/ai/transposition.h
        ../main/ai/evaluation_update_strategy.cpp ../main/ai/evaluation_update_strategy.h
        ../main/ai/evaluation.h ../main/ai/evaluation.cpp
        string_util.h string_util.cpp
)
target_link_libraries(all_tests gtest_main ${BOOST_LIBRARIES} TBB::tbb)
