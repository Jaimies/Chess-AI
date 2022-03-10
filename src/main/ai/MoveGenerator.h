#pragma once

#include "analysis_info.h"
#include "ai_settings.h"

namespace MoveGenerator {
    extern unsigned long positionsAnalyzed;
    extern AnalysisInfo *analysisInfo;

    Move *getBestMove(Board *board, AiSettings settings = prodAiSettings);
    long evaluate(Board *board, int depth);
}