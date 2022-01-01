#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "VectorUtil.cpp"
#include "Piece.h"
#include "Board.cpp"
#include <QDrag>
#include <QLabel>
#include <QMimeData>
#include <QPaintEvent>
#include <QPainter>
#include <QVBoxLayout>
#include "icon.h"
#include "ui_piece.h"

class Icon;

std::array<Icon *, 64> possibleMoveIcons;
Board *board = Board::fromFenString(Board::startPosition);

class DragWidget : public QFrame {
public:
    explicit DragWidget(QWidget *parent = nullptr) {
        setAcceptDrops(true);
    }

protected:
    void dragEnterEvent(QDragEnterEvent *event) override {
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

    void dropEvent(QDropEvent *event) override {
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

        for(auto icon: possibleMoveIcons) {
            icon->setVisible(false);
        }

        if (!draggedIcon) return;
        draggedIcon->setVisible(true);
    }

    void mousePressEvent(QMouseEvent *event) override {
        UiPiece *child = static_cast<UiPiece *>(childAt(event->pos()));
        if (!child) return;

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

        auto possibleMoves = VectorUtil::filter(board->legalMoves, [child](auto move) {
            return move->startSquare == child->square;
        });

        for(auto move: possibleMoves) {
            possibleMoveIcons[move->targetSquare]->setVisible(true);
        }
        drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
    }

    UiPiece *draggedIcon = nullptr;
};

std::map<int, std::string> iconNames{
        {1, "king"},
        {2, "queen"},
        {3, "bishop"},
        {4, "knight"},
        {5, "rook"},
        {6, "pawn"},
};

void generatePossibleMoveMarkers(QWidget *wdg) {
    for (unsigned int square = 0; square < 64; square++) {
        int rank = square / 8;
        int file = square % 8;
        auto possibleMoveIcon = new Icon(wdg);
        possibleMoveIcon->setPixmap(QIcon(":images/circle.svg").pixmap(QSize(30, 30)));
        possibleMoveIcon->move(file * 100 + 40, rank * 100 + 40);
        possibleMoveIcon->setFixedSize(20, 20);
        possibleMoveIcon->setVisible(false);
        possibleMoveIcons[square] = possibleMoveIcon;
    }
}

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    QWidget *wdg = new DragWidget(this);

    for (unsigned int square = 0; square < 64; square++) {
        auto piece = board->squares[square];

        if (piece != Piece::None) {
            int rank = square / 8;
            int file = square % 8;
            int pieceType = Piece::getType(piece);

            auto *icon = new UiPiece(wdg, square);
            auto iconName = iconNames[pieceType];
            auto iconColor = (Piece::getColour(piece) == Piece::White) ? "white" : "black";
            auto iconPath = ":/images/" + iconName + "_" + iconColor + ".svg";
            auto pixmap = QIcon(iconPath.c_str()).pixmap(QSize(70, 70));
            icon->setPixmap(pixmap);
            icon->move(file * 100 + 15, rank * 100 + 15);
            icon->setFixedSize(70, 70);
            icon->show();
        }
    }

    generatePossibleMoveMarkers(wdg);
    setCentralWidget(wdg);
}

void MainWindow::paintEvent(QPaintEvent *event) {
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

MainWindow::~MainWindow() {
    delete ui;
}
