#include "sequential.h"
#include "ai/single_depth_move_generator.h"
#include "ai/move_generator.h"

namespace DeepEvaluationStrategy {
    int64_t Sequential::_deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha, int64_t beta) const {
        bool shouldExit = false;

        for (auto &move: moves) {
            if (shouldExit || generator->parent->analysisFinished) return alpha;
            deepEvaluateMove(board, move, depth, alpha, beta, shouldExit, strategy);
        }

        return alpha;
    }
}
