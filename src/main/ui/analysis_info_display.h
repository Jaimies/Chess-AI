#pragma once

#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include "../move/move.h"
#include "game_manager.h"
#include "../ai/analysis_info.h"

const int labelHeight = 50;

class GameManager;

class AnalysisInfoDisplay : public QWidget {
public:
    AnalysisInfoDisplay(QWidget *parent, GameManager *manager);

    void updateInfo(AnalysisInfo *info);
    void showAnalysisActive();
    void showAnalysisFinished();

protected:
    bool event(QEvent *event);

private:
    QLabel *positionCount = new QLabel(this);
    QLabel *timeElapsed = new QLabel(this);
    QLabel *depth = new QLabel(this);
    QLabel *machineMove = new QLabel(this);
    QLabel *analyzing = new QLabel(this);
    QPushButton *undo = new QPushButton(this);
};
