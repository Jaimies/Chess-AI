#pragma once

#include "../move/move.h"

struct AnalysisInfo {
    const unsigned long positionsAnalyzed;
    const int depthSearchedTo;
    const Move *move;
    long millisElapsed;

    ~AnalysisInfo() {
        delete move;
    }
};
