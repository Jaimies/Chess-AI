#pragma once

#include <QLabel>
#include <QWidget>

class AnalysisInfoDisplay : public QWidget
{
public:
    AnalysisInfoDisplay(QWidget *parent);
    void updateInfo(unsigned long long positionCount, unsigned long long millisElapsed);

private:
    QLabel *positionCount;
    QLabel *timeElapsed;
};
