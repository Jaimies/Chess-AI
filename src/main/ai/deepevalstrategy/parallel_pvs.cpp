#include "parallel_pvs.h"
#include <tbb/parallel_for.h>
#include "ai/single_depth_move_generator.h"
#include "ai/move_generator.h"

namespace DeepEvaluationStrategy {
    int64_t ParallelPvs::_deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha, int64_t beta) const {
        bool shouldExit = false;

        board->makeMoveWithoutGeneratingMoves(moves[0]);
        alpha = getEvaluation(board, depth, alpha, beta, getFirstMoveEvaluationStrategy());
        board->unmakeMove(moves[0]);

        tbb::parallel_for(tbb::blocked_range<size_t>(1, moves.size()),
                          [moves, board, depth, &shouldExit, this, &alpha, &beta](tbb::blocked_range<size_t> range) {
                              for (size_t i = range.begin(); i < range.end(); i++) {
                                  auto initialAlpha = alpha;
                                  auto boardCopy = board->copy();
                                  auto moveCopy = moves[i];
                                  boardCopy->makeMoveWithoutGeneratingMoves(moveCopy);
                                  auto nullWindowEval = getNullWindowEval(boardCopy, depth, initialAlpha);
                                  boardCopy->unmakeMove(moveCopy);

                                  if (nullWindowEval != initialAlpha) {
                                      // this move is better than the current option
                                      boardCopy->makeMoveWithoutGeneratingMoves(moveCopy);
                                      auto fullWindowEval = getEvaluation(boardCopy, depth, alpha, beta,
                                                                          generator->pvsStrategy);
                                      strategy->updateEvaluation(fullWindowEval, shouldExit, alpha, beta);
                                      boardCopy->unmakeMove(moveCopy);
                                  }

                                  if (shouldExit || generator->parent->analysisFinished) return;
                              }
                          });

        return alpha;
    }

    const Base *ParallelPvs::getFirstMoveEvaluationStrategy() const {
        return generator->pvsStrategy;
    }
}