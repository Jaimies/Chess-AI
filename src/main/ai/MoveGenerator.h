#pragma once

#include "analysis_info.h"

namespace MoveGenerator {
    extern unsigned long positionsAnalyzed;
    extern AnalysisInfo *analysisInfo;

    Move *getBestMove(Board *board);
    long evaluate(Board *board, int depth);
}