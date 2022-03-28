#pragma once

#include <cstdint>
#include <vector>
#include <tbb/parallel_for.h>
#include <mutex>
#include "../board/move.h"
#include "transpositions.h"
#include "move_sorting.h"
#include "evaluation_update_strategy.h"

class DeepEvaluationStrategy {
public:
    int64_t deepEvaluate(Board *board, int depth, int64_t alpha, int64_t beta);

protected:
    void deepEvaluateMove(
            Board *board, MoveVariant move, int depth,
            int64_t &alpha, int64_t &beta, bool &shouldExit, EvaluationUpdateStrategy *strategy);

    virtual int64_t getEvaluation(Board *board, int depth, int64_t alpha, int64_t beta) = 0;

protected:
    DeepEvaluationStrategy *sequentialDeepEvaluationStrategy;

private:
    virtual int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha, int64_t beta) = 0;
};

class DeepEvaluationStrategyWithTranspositions : public DeepEvaluationStrategy {
protected:
    explicit DeepEvaluationStrategyWithTranspositions(TranspositionTable *transpositions) : transpositions(transpositions) {}

    int64_t getEvaluation(Board *board, int depth, int64_t alpha, int64_t beta) override;

    TranspositionTable *transpositions;
};

class SequentialDeepEvaluationStrategy : public DeepEvaluationStrategyWithTranspositions {
public:
    explicit SequentialDeepEvaluationStrategy(TranspositionTable *transpositions);

protected:
    NonParallelizedUpdateStrategy *strategy = new NonParallelizedUpdateStrategy();
    int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha, int64_t beta) override;
};

class ParallelDeepEvaluationStrategy : public DeepEvaluationStrategyWithTranspositions {
public:
    explicit ParallelDeepEvaluationStrategy(TranspositionTable *transpositions);

protected:
    ParallelizedUpdateStrategy *strategy = new ParallelizedUpdateStrategy();
    int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha, int64_t beta) override;
};
