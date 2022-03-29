#pragma once

#include <cstdint>
#include <vector>
#include <tbb/parallel_for.h>
#include <mutex>
#include "../board/move.h"
#include "transpositions.h"
#include "move_sorting.h"
#include "evaluation_update_strategy.h"

void deepEvaluateMove(
        Board *board, MoveVariant move, int depth, TranspositionTable *transpositions,
        int64_t &alpha, int64_t &beta, bool &shouldExit, EvaluationUpdateStrategy *strategy);

class DeepEvaluationStrategy {
public:
    int64_t deepEvaluate(Board *board, int depth, TranspositionTable *transpositions, int64_t alpha, int64_t beta);

private:
    virtual int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, TranspositionTable *transpositions, int64_t alpha, int64_t beta) = 0;
};

class _SequentialDeepEvaluationStrategy : public DeepEvaluationStrategy {
    NonParallelizedUpdateStrategy *strategy = new NonParallelizedUpdateStrategy();

    int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, TranspositionTable *transpositions, int64_t alpha, int64_t beta) override;
};


class _ParallelDeepEvaluationStrategy : public DeepEvaluationStrategy {
    ParallelizedUpdateStrategy *strategy = new ParallelizedUpdateStrategy();

    int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, TranspositionTable *transpositions, int64_t alpha, int64_t beta) override;
};

extern _SequentialDeepEvaluationStrategy *SequentialDeepEvaluationStrategy;
extern _ParallelDeepEvaluationStrategy *ParallelDeepEvaluationStrategy;
