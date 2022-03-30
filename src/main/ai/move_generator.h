#pragma once

#include "analysis_info.h"
#include "ai_settings.h"
#include "deep_evaluation_strategy.h"
#include "constants.h"

namespace MoveGenerator {
    extern unsigned long positionsAnalyzed;
    extern AnalysisInfo *analysisInfo;

    int64_t deepEvaluate(
            Board *board, int depth, const DeepEvaluationStrategy::Base *strategy,
            TranspositionTable *transpositions,
            int64_t alpha = minEvaluation, int64_t beta = maxEvaluation);
    Move *getBestMove(Board *board, AiSettings settings = prodAiSettings);
    long evaluate(Board *board, int depth);
}