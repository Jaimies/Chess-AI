#pragma once

#include "base.h"

namespace DeepEvaluationStrategy {
    class Parallel : public Base {
    public:
        explicit Parallel(SingleDepthMoveGenerator *generator): Base(generator) {}

        ParallelizedUpdateStrategy *strategy = new ParallelizedUpdateStrategy();

        int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha, int64_t beta) const override;
    };
}
