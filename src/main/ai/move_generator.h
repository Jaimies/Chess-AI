#pragma once

#include "analysis_info.h"
#include "ai_settings.h"
#include "deep_evaluation_strategy.h"
#include "constants.h"
#include "move_evaluation_data.h"
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
    int64_t deepEvaluate(
            Board *board, int depth, const DeepEvaluationStrategy::Base *strategy,
            TranspositionTable *transpositions,
            int64_t alpha = minEvaluation, int64_t beta = maxEvaluation);

    const Sequential * const sequentialStrategy = new Sequential(this);
    const Parallel * const parallelStrategy = new Parallel(this);
    const Pvs * const pvsStrategy = new Pvs(this);
    const ParallelPvs * const parallelPvsStrategy = new ParallelPvs(this);
    const ParallelPvsWithSequentialChildren * const parallelPvsWithSequentialChildrenStrategy = new ParallelPvsWithSequentialChildren(this);

    ~MoveGenerator() {
        thread->join();

        delete thread;
        delete sequentialStrategy;
        delete parallelStrategy;
        delete pvsStrategy;
        delete parallelPvsStrategy;
        delete parallelPvsWithSequentialChildrenStrategy;
    }
private:
    steady_clock::time_point begin = steady_clock::now();
    std::thread *thread = nullptr;
    Move *bestMove = nullptr;

    int64_t getFirstMoveAlpha(Board *board, int depth, std::vector<MoveVariant> moves, TranspositionTable *transpositions);
    int64_t getDeepEvaluation(Board *board, int depth, int64_t lowerBound, int64_t upperBound, TranspositionTable *transpositions);
    void evaluateMove(MoveEvaluationData *data, MoveVariant move, TranspositionTable *transpositions);
    Move *_getBestMove(Board *board, int depth, Move *supposedBestMove, AiSettings settings);
    long searchCaptures(Board *board, long alpha, long beta);
};