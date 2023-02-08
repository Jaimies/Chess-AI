#pragma once

#include "ai/deepevalstrategy/base.h"
#include "board/board.h"
#include "ai/evaluation_update_strategy.h"

class SingleDepthMoveGenerator;

namespace DeepEvaluationStrategy {
    class Sequential : public Base {
    public:
        explicit Sequential(SingleDepthMoveGenerator *generator): Base(generator) {}
    protected:
        NonParallelizedUpdateStrategy *strategy = new NonParallelizedUpdateStrategy();
    private:
        int64_t _deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha, int64_t beta) const override;
    };
}
