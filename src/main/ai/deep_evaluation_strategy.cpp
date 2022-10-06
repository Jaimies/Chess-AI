#include "deep_evaluation_strategy.h"
#include "transposition_table.h"
#include "../board/zobrist_hash_generator.h"
#include "single_depth_move_generator.h"
#include "evaluation.h"
#include "search_captures.h"
#include <iostream>

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

        AtomicTranspositionPtr ptr(new std::atomic<Transposition>({evaluation, depth, getNodeType(evaluation, alpha, beta)}));
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

    int64_t
    Sequential::_deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha, int64_t beta) const {
        bool shouldExit = false;

        for (auto &move: moves) {
            if (shouldExit || generator->parent->analysisFinished) return alpha;
            deepEvaluateMove(board, move, depth, alpha, beta, shouldExit, strategy);
        }

        return alpha;
    }

    int64_t
    Parallel::_deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha, int64_t beta) const {
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

    int64_t Base::getNullWindowEval(Board *board, int depth, int64_t alpha) const {
        return getEvaluation(board, depth, alpha, alpha + 1, generator->sequentialStrategy);
    }

    int64_t Pvs::_deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha,
                               int64_t beta) const {
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

    int64_t
    ParallelPvs::_deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha,
                               int64_t beta) const {
        bool shouldExit = false;

        board->makeMoveWithoutGeneratingMoves(moves[0]);
        alpha = getEvaluation(board, depth, alpha, beta, getFirstMoveEvaluationStrategy());
        board->unmakeMove(moves[0]);

        tbb::parallel_for(tbb::blocked_range<size_t>(1, moves.size()), [moves, board, depth, &shouldExit, this, &alpha, &beta](tbb::blocked_range<size_t> range) {
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
                    auto fullWindowEval = getEvaluation(boardCopy, depth,alpha, beta, generator->pvsStrategy);
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

    const Base *ParallelPvsWithSequentialChildren::getFirstMoveEvaluationStrategy() const {
        return generator->sequentialStrategy;
    }
}
