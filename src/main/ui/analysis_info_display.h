#pragma once

#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include "../board/Move.h"
#include "game_manager.h"

const int labelHeight = 50;

class GameManager;

class AnalysisInfoDisplay : public QWidget {
public:
    AnalysisInfoDisplay(QWidget *parent, GameManager *manager);

    void updateInfo(unsigned long long positionCount, unsigned long long millisElapsed, int depth, Move *machineMove);
    void showAnalysisActive();
    void showAnalysisFinished();

private:
    QLabel *positionCount = new QLabel(this);
    QLabel *timeElapsed = new QLabel(this);
    QLabel *depth = new QLabel(this);
    QLabel *machineMove = new QLabel(this);
    QLabel *analyzing = new QLabel(this);
    QPushButton *undo = new QPushButton(this);
};
