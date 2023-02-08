#pragma once

#include "sequential.h"

class SingleDepthMoveGenerator;

namespace DeepEvaluationStrategy {
    class Pvs: public Sequential {
    public:
        explicit Pvs(SingleDepthMoveGenerator *generator): Sequential(generator) {}
    protected:
        int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha, int64_t beta) const override;
    };
}
