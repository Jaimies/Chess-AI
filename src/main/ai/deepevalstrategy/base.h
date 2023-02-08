#pragma once

#include <cstdint>
#include "board/board.h"
#include "ai/constants.h"
#include "ai/evaluation_update_strategy.h"

class SingleDepthMoveGenerator;

namespace DeepEvaluationStrategy {
    class Base {
    public:
        int64_t deepEvaluate(Board *board, int depth, int64_t alpha = EvalValues::min, int64_t beta = EvalValues::max) const;

    protected:
        SingleDepthMoveGenerator *generator;

        explicit Base(SingleDepthMoveGenerator *generator) : generator(generator) {}

        int64_t getEvaluation(
                Board *board, int depth, int64_t alpha, int64_t beta,
                const Base *furtherEvaluationStrategy
        ) const;

        int64_t getNullWindowEval(Board *board, int depth, int64_t alpha) const;

        void deepEvaluateMove(
                Board *board, MoveVariant move, int depth,
                int64_t &alpha, int64_t &beta, bool &shouldExit, EvaluationUpdateStrategy *strategy) const;

    private:
        virtual int64_t _deepEvaluate(Board *board, std::vector <MoveVariant> moves, int depth, int64_t alpha, int64_t beta) const = 0;
    };
}
