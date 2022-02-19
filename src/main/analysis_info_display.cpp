#include "analysis_info_display.h"

#include <QLabel>
#include <string>

AnalysisInfoDisplay::AnalysisInfoDisplay(QWidget *parent) : QWidget(parent) {
    positionCount = new QLabel(this);
    positionCount->setFixedSize(400, 80);
    timeElapsed = new QLabel(this);
    timeElapsed->setFixedSize(400, 80);
    timeElapsed->move(0, 80);
    depth = new QLabel(this);
    depth->setFixedSize(400, 80);
    depth->move(0, 160);
}

void AnalysisInfoDisplay::updateInfo(unsigned long long positionCount, unsigned long long millisElapsed, int depth) {
    this->positionCount->setText(("Evaluated " + std::to_string(positionCount) + " positions").c_str());
    this->timeElapsed->setText(("Time elapsed: " + std::to_string(millisElapsed) + "ms").c_str());
    this->depth->setText(("Depth: " + std::to_string(depth)).c_str());
}
