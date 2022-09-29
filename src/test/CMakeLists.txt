cmake_minimum_required(VERSION 3.20)
project(Chess)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED)
find_package(TBB CONFIG REQUIRED)

FIND_LIBRARY(GCCLIBATOMIC_LIBRARY NAMES atomic atomic.so.1 libatomic.so.1
        HINTS
        $ENV{HOME}/local/lib64
        $ENV{HOME}/local/lib
        /usr/local/lib64
        /usr/local/lib
        /opt/local/lib64
        /opt/local/lib
        /usr/lib64
        /usr/lib
        /lib64
        /lib
        )

IF (GCCLIBATOMIC_LIBRARY)
    SET(GCCLIBATOMIC_FOUND TRUE)
    MESSAGE(STATUS "Found GCC's libatomic.so: lib=${GCCLIBATOMIC_LIBRARY}")
ELSE ()
    SET(GCCLIBATOMIC_FOUND FALSE)
    MESSAGE(STATUS "GCC's libatomic.so not found. This library is essential in AArch64 (if you are on x86, this doesn't matter)")
    MESSAGE(STATUS "Try: 'sudo yum install libatomic' (or sudo apt-get install libatomic1)")
ENDIF ()

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
        all_tests ../main/board/board.cpp ../main/move/move.cpp ../main/board/piece.cpp
        ../main/board/zobrist_hash_generator.cpp ../main/ai/move_generator.cpp board/move_count_test.cpp
        ai/move_generator_test.cpp board/move_test.cpp board/zobrist_hash_generator_test.cpp util/string_util_test.cpp
        util/vector_util_test.cpp ai/deep_evaluation_strategy_test.cpp
        ../main/board/move_processor.h ../main/board/move_processor.cpp
        ../main/ai/square_value_tables.h ../main/ai/square_value_tables.cpp
        ../main/ai/analysis_info.h ../main/ai/ai_settings.h
        ../main/util/vector_util.h ../main/util/thread_util.h
        ../main/ai/constants.h ../main/ai/constants.cpp
        ../main/ai/deep_evaluation_strategy.h ../main/ai/deep_evaluation_strategy.cpp
        ../main/ai/move_sorting.h ../main/ai/move_sorting.cpp
        ../main/ai/transposition_table.h ../main/ai/transposition.h
        ../main/ai/evaluation_update_strategy.cpp ../main/ai/evaluation_update_strategy.h
        ../main/ai/evaluation.h ../main/ai/evaluation.cpp
        string_util.h string_util.cpp
        ../main/move/visitors.cpp ../main/move/visitors.h
        ../main/ai/single_depth_move_generator.cpp ../main/ai/single_depth_move_generator.h
        ../main/ai/search_captures.cpp ../main/ai/search_captures.h
        ../main/ai/evalstrategy/sequential_deep_evaluation_strategy.cpp ../main/ai/evalstrategy/sequential_deep_evaluation_strategy.h
        ../main/ai/transposition_table.cpp ../main/board/board_util.cpp ../main/board/board_util.h board/board_util_test.cpp board/board_unit_test.cpp board/board_integrated_test.cpp)
target_link_libraries(all_tests gtest_main ${BOOST_LIBRARIES} TBB::tbb atomic)
