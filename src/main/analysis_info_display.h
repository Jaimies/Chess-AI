#pragma once

#include <QLabel>
#include <QWidget>
#include "Move.h"

class AnalysisInfoDisplay : public QWidget
{
public:
    AnalysisInfoDisplay(QWidget *parent);
    void updateInfo(unsigned long long positionCount, unsigned long long millisElapsed, int depth, Move *machineMove);

private:
    QLabel *positionCount;
    QLabel *timeElapsed;
    QLabel *depth;
    QLabel *machineMove;
};
