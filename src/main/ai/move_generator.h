#pragma once

#include "analysis_info.h"
#include "ai_settings.h"
#include "deep_evaluation_strategy.h"
#include "constants.h"
#include <thread>

using namespace DeepEvaluationStrategy;
using namespace std::chrono;

class MoveGenerator {
public:
    unsigned long positionsAnalyzed = 0;
    AnalysisInfo *analysisInfo = nullptr;
    bool analysisFinished = false;

    Move *getBestMove(Board *board, AiSettings settings = prodAiSettings);
    static long evaluate(Board *board, int depth);

    ~MoveGenerator() {
        thread->join();

        delete thread;
    }
private:
    steady_clock::time_point begin = steady_clock::now();
    std::thread *thread = nullptr;
    Move *bestMove = nullptr;
};