#pragma once

#include <QLabel>
#include <QWidget>

class AnalysisInfoDisplay : public QWidget
{
public:
    AnalysisInfoDisplay(QWidget *parent);
    void updateInfo(unsigned long long positionCount, unsigned long long millisElapsed, int depth);

private:
    QLabel *positionCount;
    QLabel *timeElapsed;
    QLabel *depth;
};
