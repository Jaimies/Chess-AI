cmake_minimum_required(VERSION 3.5)

project(Chess VERSION 0.1 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH $ENV{HOME}/Qt/6.4.2/gcc_64/)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(src/main)

find_package(Qt6 COMPONENTS Widgets LinguistTools REQUIRED)
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


set(TS_FILES Chess_en_CA.ts)

set(PROJECT_SOURCES
        src/main/board/board.cpp src/main/board/board.h
        src/main/move/move.cpp src/main/move/move.h
        src/main/board/piece.cpp src/main/board/piece.h
        src/main/util/vector_util.h
        src/main/ui/analysis_info_display.cpp src/main/ui/analysis_info_display.h
        src/main/ui/chessboard.cpp src/main/ui/chessboard.h
        src/main/ui/game_manager.cpp src/main/ui/game_manager.h
        src/main/ui/icon.cpp src/main/ui/icon.h
        src/main/main.cpp
        src/main/ui/mainwindow.cpp src/main/ui/mainwindow.h
        src/main/ui/promotion_dialog.cpp src/main/ui/promotion_dialog.h
        src/main/ui/piece_ui.cpp src/main/ui/piece_ui.h
        src/main/board/zobrist_hash_generator.cpp src/main/board/zobrist_hash_generator.h
        src/main/ai/move_generator.cpp src/main/ai/move_generator.h
        src/main/board/move_processor.h src/main/board/move_processor.cpp
        src/main/ai/square_value_tables.h src/main/ai/square_value_tables.cpp
        src/main/ai/analysis_info.h src/main/ai/ai_settings.h
        src/main/ai/constants.h
        src/main/ai/evaluation_update_strategy.h src/main/ai/evaluation_update_strategy.cpp
        src/main/ai/move_sorting.h src/main/ai/move_sorting.cpp
        src/main/ai/transposition_table.h src/main/ai/transposition.h
        src/main/ai/evaluation.h src/main/ai/evaluation.cpp
        src/main/move/visitors.cpp src/main/move/visitors.h
        src/main/ai/search_captures.cpp src/main/ai/search_captures.h
        src/main/ai/single_depth_move_generator.cpp src/main/ai/single_depth_move_generator.h
        src/main/board/board_util.cpp src/main/board/board_util.h
        src/main/board/board_squares.h src/main/board/board_squares.cpp
        resources.qrc
        ${TS_FILES}
        src/main/ai/deepevalstrategy/base.cpp src/main/ai/deepevalstrategy/base.h
        src/main/ai/deepevalstrategy/sequential.cpp src/main/ai/deepevalstrategy/sequential.h
        src/main/ai/deepevalstrategy/parallel.cpp src/main/ai/deepevalstrategy/parallel.h
        src/main/ai/deepevalstrategy/pvs.cpp src/main/ai/deepevalstrategy/pvs.h
        src/main/ai/deepevalstrategy/parallel_pvs.cpp src/main/ai/deepevalstrategy/parallel_pvs.h
        src/main/ai/deepevalstrategy/parallel_pvs_with_sequential_children.cpp src/main/ai/deepevalstrategy/parallel_pvs_with_sequential_children.h
        )

qt_add_executable(Chess
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_compile_definitions(Chess PRIVATE QT_NO_KEYWORDS)
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Chess APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

target_link_libraries(Chess PRIVATE Qt6::Widgets ${BOOST_LIBRARIES} TBB::tbb atomic)

set_target_properties(Chess PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

qt_finalize_executable(Chess)
