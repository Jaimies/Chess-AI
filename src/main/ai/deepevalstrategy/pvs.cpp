#include "pvs.h"
#include "ai/single_depth_move_generator.h"
#include "ai/move_generator.h"

namespace DeepEvaluationStrategy {
    int64_t Pvs::_deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha, int64_t beta) const {
        bool shouldExit = false;

        board->makeMoveWithoutGeneratingMoves(moves[0]);
        alpha = getEvaluation(board, depth, alpha, beta, generator->pvsStrategy);
        board->unmakeMove(moves[0]);

        for (size_t i = 1; i < moves.size(); i++) {
            board->makeMoveWithoutGeneratingMoves(moves[i]);
            auto nullWindowEval = getNullWindowEval(board, depth, alpha);
            board->unmakeMove(moves[i]);

            if (nullWindowEval != alpha) {
                // this move is better than the current option
                board->makeMoveWithoutGeneratingMoves(moves[i]);
                auto fullWindowEval = getEvaluation(board, depth, alpha, beta, generator->pvsStrategy);
                strategy->updateEvaluation(fullWindowEval, shouldExit, alpha, beta);
                board->unmakeMove(moves[i]);
            }

            if (shouldExit || generator->parent->analysisFinished) return alpha;
        }

        return alpha;
    }
}
