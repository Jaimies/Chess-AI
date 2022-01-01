#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Piece.h"
#include "Board.cpp"
#include <QDrag>
#include <QLabel>
#include <QMimeData>
#include <QPaintEvent>
#include <QPainter>
#include <QVBoxLayout>

class Icon : public QFrame
{
public:
    ~Icon() {};

    Icon(QWidget *parent): QFrame(parent) {};
    void setPixmap(const QPixmap& pixmap) {
        mPixmap = pixmap;
        update();
    }
protected:
    void paintEvent(QPaintEvent *event) {
        if (mPixmap.isNull()) {
            return;
        }
        double width = this->width();
        double height = this->height();
        double pixmapWidth = mPixmap.width();
        double pixmapHeight = mPixmap.height();
        double scale = qMax(width / pixmapWidth, height / pixmapHeight);
        QTransform transform;
        transform.translate(width / 2, height / 2);
        transform.scale(scale, scale);
        transform.translate(-pixmapWidth / 2, -pixmapHeight / 2);
        QPainter painter(this);
        painter.setTransform(transform);
        painter.drawPixmap(QPoint(0,0), mPixmap);
    };

    void dragEnterEvent(QDragEnterEvent *event) override {

    };
    void dragMoveEvent(QDragMoveEvent *event) override {

    };
    void dropEvent(QDropEvent *event) override {

    };
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            QDrag *drag = new QDrag(this);
            QMimeData *mimeData = new QMimeData();

            drag->setMimeData(mimeData);
            drag->setPixmap(this->mPixmap);
            drag->setHotSpot(QPoint(this->width() / 2, this->height() / 2));

            Qt::DropAction dropAction = drag->exec();
         }
    };

    QPixmap mPixmap;
};

Board *board = Board::fromFenString(Board::startPosition);

std::map<int, std::string> iconNames {
    {1, "king"},
    {2, "queen"},
    {3, "bishop"},
    {4, "knight"},
    {5, "rook"},
    {6, "pawn"},
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QWidget * wdg = new QWidget(this);

    for(unsigned int square = 0; square < 64; square++) {
        auto piece = board->squares[square];

        if(piece != Piece::None) {
            int rank = square / 8;
            int file = square % 8;
            int pieceType = Piece::getType(piece);

            Icon *icon = new Icon(wdg);
            auto iconName = iconNames[pieceType];
            auto iconColor = (Piece::getColour(piece) == Piece::White)? "white" : "black";
            auto iconPath = ":/images/" + iconName + "_" + iconColor + ".svg";
            auto pixmap = QPixmap(QIcon(iconPath.c_str()).pixmap(QSize(70, 70)));
            icon->setPixmap(pixmap);

            icon->move(file * 100 + 15, rank * 100 + 15);
            icon->setFixedSize(70, 70);
        }
    }

    setCentralWidget(wdg);
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    for(unsigned int square = 0; square < 64; square++) {
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

MainWindow::~MainWindow()
{
    delete ui;
}

