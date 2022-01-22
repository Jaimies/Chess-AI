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
find_package(folly CONFIG REQUIRED)

set(TS_FILES Chess_en_CA.ts)

set(PROJECT_SOURCES
        src/main/Board.cpp src/main/Board.h
        src/main/Move.cpp src/main/Move.h
        src/main/Piece.cpp src/main/Piece.h
        src/main/VectorUtil.h
        src/main/analysis_info_display.cpp src/main/analysis_info_display.h
        src/main/chessboard.cpp src/main/chessboard.h
        src/main/game_manager.cpp src/main/game_manager.h
        src/main/icon.cpp src/main/icon.h
        src/main/main.cpp
        src/main/mainwindow.cpp src/main/mainwindow.h
        src/main/promotion_dialog.cpp src/main/promotion_dialog.h
        src/main/piece_ui.cpp src/main/piece_ui.h
        src/main/zobrist_hash_generator.cpp src/main/zobrist_hash_generator.h

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

target_link_libraries(Chess PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Folly::folly)

set_target_properties(Chess PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Chess)
endif()
