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
    QLabel *positionCount = new QLabel(this);
    QLabel *timeElapsed = new QLabel(this);
    QLabel *depth = new QLabel(this);
    QLabel *machineMove = new QLabel(this);
};
