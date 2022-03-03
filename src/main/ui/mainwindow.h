#pragma once
#include "analysis_info_display.h"

#include <QMainWindow>

class ChessBoardWidget;

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    ChessBoardWidget *chessBoard;
    AnalysisInfoDisplay *info;
};
