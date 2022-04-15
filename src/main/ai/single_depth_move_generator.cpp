#include "single_depth_move_generator.h"
#include "../util/thread_util.h"
#include "../util/vector_util.h"
#include "move_generator.h"

Move *SingleDepthMoveGenerator::getBestMove(Move *supposedBestMove, AiSettings settings) {
    if (board->legalMoves.empty()) return nullptr;

    auto moves = getSortedMoves(supposedBestMove);

    int64_t alpha = getFirstMoveAlpha(moves);
    bestEvaluation = alpha;
    bestMove = moves[0];

    tbb::parallel_for(tbb::blocked_range<size_t>(1, moves.size()), [moves, &alpha, this](tbb::blocked_range<size_t> range) {
        Board * boardCopy = this->board->copy();
        for (size_t i = range.begin(); i < range.end(); i++) {
            auto initialAlpha = alpha;
            auto moveCopy = moves[i];
            boardCopy->makeMoveWithoutGeneratingMoves(moveCopy);
            auto eval = getDeepEvaluation(boardCopy, initialAlpha, alpha + 1);
            boardCopy->unmakeMove(moveCopy);
            if (eval != initialAlpha)  {
                evaluateMove(moves[i]);
                alpha = bestEvaluation;
                if (parent->analysisFinished) return;
            };
            if (parent->analysisFinished) return;
        }
        delete boardCopy;
    });

    deleteInTheBackground(transpositions);

    auto bestMove = this->bestMove.value();
    return visit(GetMovePointerVisitor, bestMove);
}

void SingleDepthMoveGenerator::evaluateMove(MoveVariant move) {
    auto boardCopy = board->copy();
    boardCopy->makeMoveWithoutGeneratingMoves(move);
    auto evaluation = -parallelPvsStrategy->deepEvaluate(boardCopy, depth);
    boardCopy->unmakeMove(move);

    mutex.lock();
    if (evaluation > bestEvaluation) {
        bestEvaluation = evaluation;
        bestMove = move;
    }
    mutex.unlock();

    delete boardCopy;
}

std::vector<MoveVariant> SingleDepthMoveGenerator::getSortedMoves(Move *supposedBestMove) const {
    auto moves = board->legalMoves;
    if (!supposedBestMove) {
        sortMoves(board, moves);
    } else {
        auto supposedBestMoveIndex = VectorUtil::indexOf(moves, supposedBestMove->toVariant());
        VectorUtil::move(moves, supposedBestMoveIndex, 0);
    }

    return moves;
}

int64_t SingleDepthMoveGenerator::getDeepEvaluation(Board *board, int64_t lowerBound, int64_t upperBound) const {
    return -parallelStrategy->deepEvaluate(board, depth, -upperBound, -lowerBound);
}

int64_t SingleDepthMoveGenerator::getFirstMoveAlpha(std::vector<MoveVariant> moves) const {
    board->makeMoveWithoutGeneratingMoves(moves[0]);
    int64_t firstMoveAlpha = -parallelPvsStrategy->deepEvaluate(board, depth, minEvaluation, maxEvaluation);
    board->unmakeMove(moves[0]);
    return firstMoveAlpha;
}