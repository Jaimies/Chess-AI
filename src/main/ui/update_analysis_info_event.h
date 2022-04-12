#pragma once

#include <QEvent>
#include "../ai/analysis_info.h"

class UpdateAnalysisInfoEvent : public QEvent {
public:
    UpdateAnalysisInfoEvent(AnalysisInfo *info) : QEvent(type), info(info) {};
    ~UpdateAnalysisInfoEvent() {};

    AnalysisInfo *info;

    static const QEvent::Type type = (QEvent::Type) 1001;
};
