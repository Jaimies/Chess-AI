#include "analysis_info_display.h"

#include <QLabel>
#include <string>
#include <iostream>
#include <QObject>
#include <QApplication>
#include "update_analysis_info_event.h"
#include "../ai/move_generator.h"

AnalysisInfoDisplay::AnalysisInfoDisplay(QWidget *parent, GameManager *manager) : QWidget(parent) {
    positionCount->setFixedSize(400, labelHeight);

    timeElapsed->setFixedSize(400, labelHeight);
    timeElapsed->move(0, labelHeight);

    depth->setFixedSize(400, labelHeight);
    depth->move(0, labelHeight * 2);

    machineMove->setFixedSize(400, labelHeight);
    machineMove->move(0, labelHeight * 3);

    analyzing->setFixedSize(400, labelHeight);
    analyzing->move(0, labelHeight * 4);

    undo->setText("Undo");
    undo->move(0, labelHeight * 5);
    undo->setFixedSize(400, labelHeight);

    QObject::connect(undo, &QPushButton::clicked, [manager]() {
        manager->undoLastMove();
    });
}

void AnalysisInfoDisplay::updateInfo(AnalysisInfo *info) {
    QApplication::postEvent(this, new UpdateAnalysisInfoEvent());
}

void AnalysisInfoDisplay::showAnalysisActive() {
    positionCount->setText("");
    timeElapsed->setText("");
    depth->setText("");
    machineMove->setText("");
    analyzing->setVisible(true);
    analyzing->setText("Analyzing...............................................................................................");
}

void AnalysisInfoDisplay::showAnalysisFinished() {
    analyzing->setVisible(false);
}

bool AnalysisInfoDisplay::event(QEvent *event)
{
    if (event->type() != UpdateAnalysisInfoEvent::type) {
        return QWidget::event(event);
    }

    auto info = MoveGenerator::analysisInfo;
    if (!info) return true;

    this->positionCount->setText(("Evaluated " + std::to_string(info->positionsAnalyzed) + " positions").c_str());
    this->timeElapsed->setText(("Time elapsed: " + std::to_string(info->millisElapsed) + "ms").c_str());
    this->depth->setText(("Depth: " + std::to_string(info->depthSearchedTo)).c_str());
    this->machineMove->setText(("Last move: " + info->move->toString()).c_str());
    showAnalysisFinished();

    delete info->move;
    delete info;

    MoveGenerator::analysisInfo = nullptr;

    return true;
}
