#pragma once

#include "parallel.h"

class SingleDepthMoveGenerator;

namespace DeepEvaluationStrategy {
    class ParallelPvs : public Parallel {
    public:
        explicit ParallelPvs(SingleDepthMoveGenerator *generator) : Parallel(generator) {}

    protected:
        virtual const Base *getFirstMoveEvaluationStrategy() const;

        int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha, int64_t beta) const override;
    };
}
