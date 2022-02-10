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

void ChessBoardWidget::generatePossibleMoveMarkers() {
    for (unsigned int square = 0; square < 64; square++) {
        int rank = square / 8;
        int file = square % 8;
        auto possibleMoveIcon = new Icon(this);
        possibleMoveIcon->setPixmap(QIcon(":images/circle.svg").pixmap(QSize(30, 30)));
        possibleMoveIcon->move(700 - file * 100 + 40, rank * 100 + 40);
        possibleMoveIcon->setFixedSize(20, 20);
        possibleMoveIcon->setVisible(false);
        possibleMoveIcons[square] = possibleMoveIcon;
    }
}

std::array<Move *, 64> moves{nullptr};

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

void ChessBoardWidget::dropEvent(QDropEvent *event) {
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

    for (auto icon: possibleMoveIcons) {
        icon->setVisible(false);
    }

    int file = 7 - event->pos().x() / 100;
    int rank = event->pos().y() / 100;

    if (!draggedIcon || !isValidCoordinate(file) || !isValidCoordinate(rank)) return;

    int square = rank * 8 + file;

    draggedIcon->setVisible(true);

    auto move = moves[square];
    if (move) gameManager->makeMove(move);

    for (int square = 0; square < 64; square++) moves[square] = nullptr;
}

void ChessBoardWidget::mousePressEvent(QMouseEvent *event) {
    UiPiece *child = static_cast<UiPiece *>(childAt(event->pos()));
    if (!child || !child->isDraggable
        || Piece::getColour(gameManager->board->squares[child->getSquare()]) != gameManager->board->colourToMove)
        return;

    this->draggedIcon = child;

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

    auto possibleMoves = VectorUtil::filter(gameManager->board->legalMoves, [child](auto move) {
        return move->startSquare == child->getSquare();
    });

    for (auto move: possibleMoves) {
        auto icons = possibleMoveIcons;
        moves[move->targetSquare] = move;
        possibleMoveIcons[move->targetSquare]->setVisible(true);
    }
    drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
}

void ChessBoardWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    for (unsigned int square = 0; square < 64; square++) {
        auto rank = square / 8;
        auto file = square % 8;
        bool isWhiteCell = (rank + file) % 2 == 0;
        auto color = isWhiteCell
                     ? QColor(247, 223, 176)
                     : QColor(163, 112, 67);

        painter.setBrush(color);

        QRect r(rank * 100, file * 100, 100, 100);
        painter.drawRect(r);
    }
}
