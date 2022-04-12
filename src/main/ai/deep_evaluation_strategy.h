#pragma once

#include <cstdint>
#include <vector>
#include <tbb/parallel_for.h>
#include <mutex>
#include "../board/move.h"
#include "transpositions.h"
#include "move_sorting.h"
#include "evaluation_update_strategy.h"

class MoveGenerator;

namespace DeepEvaluationStrategy {
    class Base {
    public:
        int64_t deepEvaluate(Board *board, int depth, TranspositionTable *transpositions, int64_t alpha, int64_t beta) const;

    protected:
        MoveGenerator *generator;

        explicit Base(MoveGenerator *generator): generator(generator) {}

        int64_t getEvaluation(
                Board *board, int depth, TranspositionTable *transpositions, int &nodeType, int64_t alpha, int64_t beta,
                const Base *const furtherEvaluationStrategy
        ) const;

        int64_t getNullWindowEval(Board *board, int depth, TranspositionTable *transpositions, int &nodeType, int64_t alpha) const;

        void deepEvaluateMove(
                Board *board, MoveVariant move, int depth, TranspositionTable *transpositions, int &nodeType,
                int64_t &alpha, int64_t &beta, bool &shouldExit, EvaluationUpdateStrategy *strategy) const;
    private:
        virtual int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, TranspositionTable *transpositions, int &nodeType, int64_t alpha, int64_t beta) const = 0;
    };

    class Sequential : public Base {
    public:
        explicit Sequential(MoveGenerator *generator): Base(generator) {}
    protected:
        NonParallelizedUpdateStrategy *strategy = new NonParallelizedUpdateStrategy();
    private:
        int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, TranspositionTable *transpositions, int &nodeType, int64_t alpha, int64_t beta) const override;
    };

    class Parallel : public Base {
    public:
        explicit Parallel(MoveGenerator *generator): Base(generator) {}

        ParallelizedUpdateStrategy *strategy = new ParallelizedUpdateStrategy();

        int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, TranspositionTable *transpositions, int &nodeType, int64_t alpha, int64_t beta) const override;
    };

    class Pvs: public Sequential {
    public:
        explicit Pvs(MoveGenerator *generator): Sequential(generator) {}
    protected:
        int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, TranspositionTable *transpositions, int &nodeType, int64_t alpha, int64_t beta) const override;
    };

    class ParallelPvs : public Parallel {
    public:
        explicit ParallelPvs(MoveGenerator *generator): Parallel(generator) {}
    protected:
        virtual const Base *const getFirstMoveEvaluationStrategy() const;
        int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, TranspositionTable *transpositions, int &nodeType, int64_t alpha, int64_t beta) const override;
    };

    class ParallelPvsWithSequentialChildren: public ParallelPvs {
    public:
        explicit ParallelPvsWithSequentialChildren(MoveGenerator *generator): ParallelPvs(generator) {}
    protected:
        const Base *const getFirstMoveEvaluationStrategy() const override;
    };
}
