#pragma once

#include <mutex>
#include "transposition_table.h"
#include "../move/move.h"
#include "ai_settings.h"
#include "constants.h"
#include "deep_evaluation_strategy.h"

class MoveGenerator;

using namespace DeepEvaluationStrategy;

class SingleDepthMoveGenerator {
public:
    explicit SingleDepthMoveGenerator(MoveGenerator *parent, Board *board, const int depth) : parent(parent), board(board), depth(depth) {}

    TranspositionTable *transpositions = new TranspositionTable();
    MoveGenerator *parent;
    Board *board;
    const int depth;
    std::optional<MoveVariant> bestMove;
    int64_t bestEvaluation = minEvaluation;
    std::mutex mutex;

    const Sequential * const sequentialStrategy = new Sequential(this);
    const Parallel * const parallelStrategy = new Parallel(this);
    const Pvs * const pvsStrategy = new Pvs(this);
    const ParallelPvs * const parallelPvsStrategy = new ParallelPvs(this);
    const ParallelPvsWithSequentialChildren * const parallelPvsWithSequentialChildrenStrategy = new ParallelPvsWithSequentialChildren(this);

    Move *getBestMove(Move *supposedBestMove, AiSettings settings);
    int64_t getFirstMoveAlpha(std::vector<MoveVariant> moves) const;
    int64_t getDeepEvaluation(Board *board, int64_t lowerBound, int64_t upperBound) const;
    void evaluateMove(MoveVariant move);
    std::vector<MoveVariant> getSortedMoves(Move *supposedBestMove) const;

    ~SingleDepthMoveGenerator() {
        delete sequentialStrategy;
        delete parallelStrategy;
        delete pvsStrategy;
        delete parallelPvsStrategy;
        delete parallelPvsWithSequentialChildrenStrategy;
    }
};
