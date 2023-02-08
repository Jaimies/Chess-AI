#include "base.h"
#include "ai/search_captures.h"
#include "ai/evaluation.h"
#include "ai/transposition.h"
#include "ai/transposition_table.h"
#include "ai/single_depth_move_generator.h"
#include "ai/move_generator.h"
#include "ai/move_sorting.h"

bool shouldUpdateTransposition(int depth, int type, const Transposition &transposition) {
    return depth > transposition.depth && transposition.type == type;
}

static int getNodeType(int64_t eval, int64_t alpha, int64_t beta) {
    if (eval >= beta) return Transposition::LOWER;
    if (eval <= alpha) return Transposition::UPPER;
    return Transposition::EXACT;
}

namespace DeepEvaluationStrategy {
    int64_t Base::getEvaluation(
            Board *board, int depth, int64_t alpha, int64_t beta,
            const DeepEvaluationStrategy::Base *furtherEvaluationStrategy
    ) const {
        auto boardHash = board->getZobristHash();

        TranspositionTable::const_accessor accessor;
        auto isFound = generator->transpositions->find(accessor, boardHash);

        if (isFound) {
            auto transposition = (Transposition) *accessor->second;

            if (transposition.depth >= depth) {
                if (transposition.type == Transposition::EXACT)
                    return transposition.value;

                if (transposition.type == Transposition::LOWER && transposition.value >= beta)
                    return beta;

                if (transposition.type == Transposition::UPPER && transposition.value <= alpha)
                    return alpha;
            }

            auto evaluation = -furtherEvaluationStrategy->deepEvaluate(board, depth - 1, -beta, -alpha);

            int nodeType = getNodeType(evaluation, alpha, beta);

            if (shouldUpdateTransposition(depth, nodeType, transposition))
                accessor->second->exchange({evaluation, depth, nodeType});

            return evaluation;
        }

        auto evaluation = -furtherEvaluationStrategy->deepEvaluate(board, depth - 1, -beta, -alpha);

        AtomicTranspositionPtr ptr(
                new std::atomic<Transposition>({evaluation, depth, getNodeType(evaluation, alpha, beta)}));
        generator->transpositions->insert({{boardHash, ptr}});

        return evaluation;
    }

    void Base::deepEvaluateMove(
            Board *board, MoveVariant move, int depth,
            int64_t &alpha, int64_t &beta, bool &shouldExit, EvaluationUpdateStrategy *strategy) const {
        board->makeMoveWithoutGeneratingMoves(move);
        auto evaluation = getEvaluation(board, depth, alpha, beta, generator->sequentialStrategy);
        board->unmakeMove(move);
        strategy->updateEvaluation(evaluation, shouldExit, alpha, beta);
    }

    int64_t Base::deepEvaluate(Board *board, int depth, int64_t alpha, int64_t beta) const {
        if (depth == 0) {
            generator->parent->positionsAnalyzed++;
            return searchCaptures(board, alpha, beta);
        }

        board->generateMoves();

        if (board->legalMoves.empty()) {
            generator->parent->positionsAnalyzed++;
            return evaluatePositionWithoutMoves(board, depth);
        }

        auto moves = std::vector(board->legalMoves);
        sortMoves(board, moves);

        return _deepEvaluate(board, moves, depth, alpha, beta);
    };

    int64_t Base::getNullWindowEval(Board *board, int depth, int64_t alpha) const {
        return getEvaluation(board, depth, alpha, alpha + 1, generator->sequentialStrategy);
    }
}
