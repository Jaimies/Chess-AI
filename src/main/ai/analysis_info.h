#pragma once

#include "../board/move.h"

struct AnalysisInfo {
    const unsigned long positionsAnalyzed;
    const int depthSearchedTo;
    const Move *move;
    long millisElapsed;
};
