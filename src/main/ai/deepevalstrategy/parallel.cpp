#include "parallel.h"
#include <tbb/parallel_for.h>
#include "ai/single_depth_move_generator.h"
#include "ai/move_generator.h"

namespace DeepEvaluationStrategy {
    int64_t Parallel::_deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha, int64_t beta) const {
        bool shouldExit = false;

        auto body = [this, board, moves, depth, &alpha, &beta, &shouldExit](tbb::blocked_range<size_t> range) {
            for (size_t i = range.begin(); i < range.end(); ++i) {
                if (shouldExit || generator->parent->analysisFinished) return;
                auto boardCopy = board->copy();
                deepEvaluateMove(boardCopy, moves[i], depth, alpha, beta, shouldExit, strategy);
                delete boardCopy;
            }
        };

        tbb::parallel_for(tbb::blocked_range<size_t>(0, moves.size()), body);

        return alpha;
    }
}
