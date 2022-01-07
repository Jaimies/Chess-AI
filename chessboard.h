#pragma once

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "VectorUtil.h"
#include "Piece.h"
#include "Board.h"
#include <QDrag>
#include <QLabel>
#include <QMimeData>
#include <QPaintEvent>
#include <QPainter>
#include <QVBoxLayout>
#include "icon.h"
#include "ui_piece.h"
#include "game_manager.h"

class ChessBoardWidget : public QFrame {
public:
    explicit ChessBoardWidget(QWidget *parent, GameManager *gameManager) : QFrame(parent), gameManager(gameManager) {
        setAcceptDrops(true);
        generatePossibleMoveMarkers();
    }

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

    UiPiece *draggedIcon = nullptr;

private:
    void generatePossibleMoveMarkers();

    std::array<Icon *, 64> possibleMoveIcons;
    GameManager *gameManager;
};
