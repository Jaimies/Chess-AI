#pragma once

#include "../board/Move.h"

class AnalysisInfo {
public:
    const unsigned long positionsAnalyzed;
    const int depthSearchedTo;
    const Move *move;
    long millisElapsed;

    AnalysisInfo(unsigned long positionsAnalyzed, int depthSearchedTo, Move *move, long millisElapsed)
            : positionsAnalyzed(positionsAnalyzed), depthSearchedTo(depthSearchedTo),
              move(move), millisElapsed(millisElapsed) {};
};
