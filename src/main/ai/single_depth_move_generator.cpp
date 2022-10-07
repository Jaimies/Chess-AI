#include "single_depth_move_generator.h"
#include "../util/thread_util.h"
#include "../util/vector_util.h"
#include "move_generator.h"

Move *SingleDepthMoveGenerator::getBestMove(Move *supposedBestMove, AiSettings settings) {
    if (board->legalMoves.empty()) return nullptr;

    auto moves = getSortedMoves(supposedBestMove);

    bestEvaluation = getFirstMoveAlpha(moves);
    bestMove = moves[0];

    tbb::parallel_for(tbb::blocked_range<size_t>(1, moves.size()), [moves, this](tbb::blocked_range<size_t> range) {
        Board *boardCopy = this->board->copy();

        for (size_t i = range.begin(); i < range.end(); i++) {
            doFullEvalIfNeeded(boardCopy, moves[i]);
            if (parent->analysisFinished) break;
        }

        delete boardCopy;
    });

    deleteInTheBackground(transpositions);

    return visit(GetMovePointerVisitor, bestMove.value());
}

void SingleDepthMoveGenerator::fullEval(MoveVariant move) {
    auto boardCopy = board->copy();
    boardCopy->makeMoveWithoutGeneratingMoves(move);
    auto evaluation = -parallelPvsStrategy->deepEvaluate(boardCopy, depth, EvalValues::min, -bestEvaluation);
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

int64_t SingleDepthMoveGenerator::nullWindowEval(Board *board, int64_t lowerBound) const {
    return getDeepEvaluation(board, lowerBound, lowerBound + 1);
}

bool SingleDepthMoveGenerator::needsFullEval(Board *board, MoveVariant &move) const {
    auto initialAlpha = this->bestEvaluation;
    board->makeMoveWithoutGeneratingMoves(move);
    auto eval = nullWindowEval(board, initialAlpha);
    board->unmakeMove(move);

    return eval != initialAlpha;
}

void SingleDepthMoveGenerator::doFullEvalIfNeeded(Board *board, MoveVariant move) {
    if (needsFullEval(board, move)) {
        fullEval(move);
    };
}

int64_t SingleDepthMoveGenerator::getFirstMoveAlpha(std::vector<MoveVariant> moves) const {
    board->makeMoveWithoutGeneratingMoves(moves[0]);
    int64_t firstMoveAlpha = -parallelPvsStrategy->deepEvaluate(board, depth, EvalValues::min, EvalValues::max);
    board->unmakeMove(moves[0]);
    return firstMoveAlpha;
}