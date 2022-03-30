#pragma once

#include <cstdint>
#include <vector>
#include <tbb/parallel_for.h>
#include <mutex>
#include "../board/move.h"
#include "transpositions.h"
#include "move_sorting.h"
#include "evaluation_update_strategy.h"

namespace DeepEvaluationStrategy {
    class Base {
    public:
        int64_t deepEvaluate(Board *board, int depth, TranspositionTable *transpositions, int64_t alpha, int64_t beta) const;

    protected:
        void deepEvaluateMove(
                Board *board, MoveVariant move, int depth, TranspositionTable *transpositions,
                int64_t &alpha, int64_t &beta, bool &shouldExit, EvaluationUpdateStrategy *strategy) const;
    private:
        virtual int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, TranspositionTable *transpositions, int64_t alpha, int64_t beta) const = 0;
    };

    class Sequential : public Base {
    public:
        static const Sequential *const Instance;
    private:
        NonParallelizedUpdateStrategy *strategy = new NonParallelizedUpdateStrategy();

        int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, TranspositionTable *transpositions, int64_t alpha, int64_t beta) const override;
    };

    class Parallel : public Base {
    public:
        static const Parallel *const Instance;

        ParallelizedUpdateStrategy *strategy = new ParallelizedUpdateStrategy();

        int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, TranspositionTable *transpositions, int64_t alpha, int64_t beta) const override;
    };
}
