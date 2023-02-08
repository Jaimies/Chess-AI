#pragma once

#include "parallel_pvs.h"

class SingleDepthMoveGenerator;

namespace DeepEvaluationStrategy {
    class ParallelPvsWithSequentialChildren : public ParallelPvs {
    public:
        explicit ParallelPvsWithSequentialChildren(SingleDepthMoveGenerator *generator) : ParallelPvs(generator) {}

    protected:
        const Base *getFirstMoveEvaluationStrategy() const override;
    };
}
