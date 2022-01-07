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
#include "chessboard.h"
#include "game_manager.h"
#include "promotion_dialog.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setWindowTitle(tr("Chess AI"));
    this->setMinimumSize(800, 800);

    auto manager = new GameManager();
    ChessBoardWidget *wdg = new ChessBoardWidget(this, manager);
    manager->setup(wdg);
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
