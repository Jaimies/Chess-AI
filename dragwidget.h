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

class DragWidget : public QFrame {
public:
    explicit DragWidget(QWidget *parent = nullptr) {
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
};
