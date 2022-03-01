#pragma once

#include "mainwindow.h"

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
#include "piece_ui.h"
#include "game_manager.h"

class ChessBoardWidget : public QFrame {
public:
    explicit ChessBoardWidget(QWidget *parent, GameManager *gameManager) : QFrame(parent), gameManager(gameManager) {
        setAcceptDrops(true);
        generatePossibleMoveMarkers();
        setFixedSize(800, 800);
    }

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

    UiPiece *draggedIcon = nullptr;

private:
    void generatePossibleMoveMarkers();
    void showPossibleMoveMarkers(int startSquare);
    void hidePossibleMoveMarkers();
    void processDropEvent(QDropEvent *event);
    void tryToMakeMove(int square);
    void setActiveSquare(UiPiece *piece, QMouseEvent *event);
    Icon *createPossibleMoveIcon(int square);
    void startDrag(UiPiece *child, QMouseEvent *event);
    std::vector<Move *> getPossibleMoves(int startSquare);
    bool shouldStartDrag(UiPiece *child);
    bool canPieceMove(int square);

    std::array<Move *, 64> moves{nullptr};
    std::array<Icon *, 64> possibleMoveIcons;
    GameManager *gameManager;
};
