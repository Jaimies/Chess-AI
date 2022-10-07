#pragma once

#include <cstdint>
#include <vector>
#include <tbb/parallel_for.h>
#include <mutex>
#include "../move/move.h"
#include "transposition_table.h"
#include "move_sorting.h"
#include "evaluation_update_strategy.h"
#include "constants.h"

class SingleDepthMoveGenerator;

namespace DeepEvaluationStrategy {
    class Base {
    public:
        int64_t deepEvaluate(Board *board, int depth, int64_t alpha = EvalValues::min, int64_t beta = EvalValues::max) const;

    protected:
        SingleDepthMoveGenerator *generator;

        explicit Base(SingleDepthMoveGenerator *generator): generator(generator) {}

        int64_t getEvaluation(
                Board *board, int depth, int64_t alpha, int64_t beta,
                const Base *furtherEvaluationStrategy
        ) const;

        int64_t getNullWindowEval(Board *board, int depth, int64_t alpha) const;

        void deepEvaluateMove(
                Board *board, MoveVariant move, int depth,
                int64_t &alpha, int64_t &beta, bool &shouldExit, EvaluationUpdateStrategy *strategy) const;
    private:
        virtual int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha, int64_t beta) const = 0;
    };

    class Sequential : public Base {
    public:
        explicit Sequential(SingleDepthMoveGenerator *generator): Base(generator) {}
    protected:
        NonParallelizedUpdateStrategy *strategy = new NonParallelizedUpdateStrategy();
    private:
        int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha, int64_t beta) const override;
    };

    class Parallel : public Base {
    public:
        explicit Parallel(SingleDepthMoveGenerator *generator): Base(generator) {}

        ParallelizedUpdateStrategy *strategy = new ParallelizedUpdateStrategy();

        int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha, int64_t beta) const override;
    };

    class Pvs: public Sequential {
    public:
        explicit Pvs(SingleDepthMoveGenerator *generator): Sequential(generator) {}
    protected:
        int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha, int64_t beta) const override;
    };

    class ParallelPvs : public Parallel {
    public:
        explicit ParallelPvs(SingleDepthMoveGenerator *generator): Parallel(generator) {}
    protected:
        virtual const Base *getFirstMoveEvaluationStrategy() const;
        int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha, int64_t beta) const override;
    };

    class ParallelPvsWithSequentialChildren: public ParallelPvs {
    public:
        explicit ParallelPvsWithSequentialChildren(SingleDepthMoveGenerator *generator): ParallelPvs(generator) {}
    protected:
        const Base *getFirstMoveEvaluationStrategy() const override;
    };
}
