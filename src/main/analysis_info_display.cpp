#include "analysis_info_display.h"

#include <QLabel>
#include <string>

AnalysisInfoDisplay::AnalysisInfoDisplay(QWidget *parent) : QWidget(parent) {
    positionCount->setFixedSize(400, 80);

    timeElapsed->setFixedSize(400, 80);
    timeElapsed->move(0, 80);

    depth->setFixedSize(400, 80);
    depth->move(0, 160);

    machineMove->setFixedSize(400, 80);
    machineMove->move(0, 240);
}

void AnalysisInfoDisplay::updateInfo(unsigned long long positionCount, unsigned long long millisElapsed, int depth, Move *machineMove) {
    this->positionCount->setText(("Evaluated " + std::to_string(positionCount) + " positions").c_str());
    this->timeElapsed->setText(("Time elapsed: " + std::to_string(millisElapsed) + "ms").c_str());
    this->depth->setText(("Depth: " + std::to_string(depth)).c_str());
    this->machineMove->setText(("Last move: " + machineMove->toString()).c_str());
}
