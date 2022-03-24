cmake_minimum_required(VERSION 3.5)

project(Chess VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src/main)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED)
find_package(TBB CONFIG REQUIRED)

set(TS_FILES Chess_en_CA.ts)

set(PROJECT_SOURCES
        src/main/board/board.cpp src/main/board/board.h
        src/main/board/move.cpp src/main/board/move.h
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
        src/main/ai/move_evaluation_data.h src/main/ai/constants.h src/main/ai/constants.cpp
        src/main/ai/deep_evaluation_strategy.h src/main/ai/deep_evaluation_strategy.cpp
        src/main/ai/evaluation_update_strategy.h src/main/ai/evaluation_update_strategy.cpp
        src/main/ai/move_sorting.h src/main/ai/move_sorting.cpp
        src/main/ai/transpositions.cpp src/main/ai/transpositions.h
        src/main/ai/transposition.h
        src/main/ai/evaluation.h src/main/ai/evaluation.cpp
        resources.qrc
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Chess
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Chess APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(Chess SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Chess
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(Chess PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${BOOST_LIBRARIES} TBB::tbb)

set_target_properties(Chess PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Chess)
endif()
