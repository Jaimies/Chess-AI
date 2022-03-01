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
#include "chessboard.h"
#include <iostream>

Icon *ChessBoardWidget::createPossibleMoveIcon(int square) {
    int rank = square / 8;
    int file = square % 8;
    auto possibleMoveIcon = new Icon(this);
    possibleMoveIcon->setPixmap(QIcon(":images/red_square.svg").pixmap(QSize(100, 100)));
    possibleMoveIcon->move(700 - file * 100, rank * 100);
    possibleMoveIcon->setFixedSize(100, 100);
    possibleMoveIcon->setVisible(false);
    return possibleMoveIcon;
}

void ChessBoardWidget::generatePossibleMoveMarkers() {
    for (unsigned int square = 0; square < 64; square++)
        possibleMoveIcons[square] = createPossibleMoveIcon(square);
}

void ChessBoardWidget::showPossibleMoveMarkers(int startSquare) {
    auto possibleMoves = getPossibleMoves(startSquare);

    for (auto move: possibleMoves) {
        auto icons = possibleMoveIcons;
        moves[move->targetSquare] = move;
        possibleMoveIcons[move->targetSquare]->setVisible(true);
    }
}

void ChessBoardWidget::hidePossibleMoveMarkers() {
    for (auto icon: possibleMoveIcons) icon->setVisible(false);
    for (int square = 0; square < 64; square++) moves[square] = nullptr;
}

void ChessBoardWidget::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

static bool isValidCoordinate(int coordinate) {
    return coordinate >= 0 && coordinate <= 7;
}

void ChessBoardWidget::processDropEvent(QDropEvent *event) {
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint offset;
        dataStream >> pixmap >> offset;

        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

int getSquare(QPointF position) {
    int file = 7 - position.x() / 100;
    int rank = position.y() / 100;

    if (!isValidCoordinate(file) || !isValidCoordinate(rank)) return -1;

    return rank * 8 + file;
}

void ChessBoardWidget::dropEvent(QDropEvent *event) {
    processDropEvent(event);

    auto square = getSquare(event->pos());
    if (!draggedIcon || square == -1) return;

    draggedIcon->setVisible(true);
    tryToMakeMove(square);
}

void ChessBoardWidget::startDrag(UiPiece *child, QMouseEvent *event) {
    QPixmap pixmap = child->pixmap();

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << pixmap << QPoint(event->pos() - child->pos());
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(event->pos() - child->pos());

    child->setVisible(false);

    drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
}

std::vector<Move *> toMoves(std::vector<MoveVariant> moves) {
    return VectorUtil::map<MoveVariant, Move *>(moves, [](MoveVariant &variant) {
        return visit(GetMovePointerVisitor(), variant);
    });
}

std::vector<Move *> ChessBoardWidget::getPossibleMoves(int startSquare) {
    auto allMoves = toMoves(gameManager->board->legalMoves);

    return VectorUtil::filter(allMoves, [startSquare](auto move) {
        return move->startSquare == startSquare;
    });
}

bool ChessBoardWidget::canPieceMove(int square) {
    auto pieceColor = Piece::getColour(gameManager->board->squares[square]);
    return pieceColor == gameManager->board->colourToMove;
}

bool ChessBoardWidget::shouldStartDrag(UiPiece *child) {
    return child && child->isDraggable
           && canPieceMove(child->getSquare());
}

void ChessBoardWidget::tryToMakeMove(int square) {
    auto move = moves[square];
    if (move != nullptr) {
        gameManager->makeMove(move);
        hidePossibleMoveMarkers();
        draggedIcon = nullptr;
        for (int square = 0; square < 64; square++) moves[square] = nullptr;
    }
}

void ChessBoardWidget::mousePressEvent(QMouseEvent *event) {
    auto child = (UiPiece *) childAt(event->pos());

    auto square = getSquare(event->pos());
    if (square == -1) return;

    if (draggedIcon) {
        hidePossibleMoveMarkers();

        if (child->getSquare() == draggedIcon->getSquare()) {
            draggedIcon = nullptr;
            return;
        }
    }

    setActiveSquare(child, event);
}

void ChessBoardWidget::setActiveSquare(UiPiece *child, QMouseEvent *event) {
    if (!shouldStartDrag(child)) return;

    this->draggedIcon = child;
    showPossibleMoveMarkers(child->getSquare());
    startDrag(child, event);
}

QColor getColor(int rank, int file) {
    bool isWhiteCell = (rank + file) % 2 == 0;
    return isWhiteCell ? QColor(244, 216, 184) : QColor(190, 134, 102);
}

QRect getRect(int rank, int file) {
    return QRect(rank * 100, file * 100, 100, 100);
}

void drawSquare(int square, QPainter &painter) {
    auto rank = square / 8;
    auto file = square % 8;

    painter.setBrush(getColor(rank, file));
    painter.drawRect(getRect(rank, file));
}

void ChessBoardWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    for (int square = 0; square < 64; square++)
        drawSquare(square, painter);
}
