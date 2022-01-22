#include "analysis_info_display.h"
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
#include <QTimer>
#include <QVBoxLayout>
#include "icon.h"
#include "piece_ui.h"
#include "chessboard.h"
#include "game_manager.h"
#include "promotion_dialog.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setWindowTitle(tr("Chess AI"));
    this->setMinimumSize(1300, 900);

    auto manager = new GameManager();
    auto rootWidget = new QWidget(this);
    info = new AnalysisInfoDisplay(rootWidget);
    chessBoard = new ChessBoardWidget(rootWidget, manager);

    manager->setup(chessBoard, info);
    setCentralWidget(rootWidget);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    auto boardYPosition = (event->size().height() - 800) / 2;
    chessBoard->move(50, boardYPosition);
    info->move(900, boardYPosition + 40);
}

MainWindow::~MainWindow() {
    delete ui;
}
