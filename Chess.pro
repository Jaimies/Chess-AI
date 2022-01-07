QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++2a

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RESOURCES = resources.qrc

SOURCES += \
    analysis_info_display.cpp \
    main.cpp \
    mainwindow.cpp \
    Board.cpp \
    Piece.cpp \
    Move.cpp \
    icon.cpp \
    promotion_dialog.cpp \
    ui_piece.cpp \
    chessboard.cpp \
    game_manager.cpp \
    MoveGenerator.cpp

HEADERS += \
    analysis_info_display.h \
    mainwindow.h \
    Move.h \
    Board.h \
    Piece.h \
    VectorUtil.h \
    icon.h \
    promotion_dialog.h \
    ui_piece.h \
    chessboard.h \
    game_manager.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    Chess_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
