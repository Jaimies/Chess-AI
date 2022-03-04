#pragma once

#include <QEvent>

class UpdateAnalysisInfoEvent : public QEvent {
public:
    UpdateAnalysisInfoEvent() : QEvent(type) {};
    ~UpdateAnalysisInfoEvent() {};

    static const QEvent::Type type = (QEvent::Type) 1001;
};
