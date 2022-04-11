#include "deep_evaluation_strategy.h"
#include "move_generator.h"
#include "transpositions.h"
#include "../board/zobrist_hash_generator.h"
#include <iostream>

bool shouldUpdateTransposition(int depth, int type, const Transposition &transposition) {
    return depth > transposition.depth && transposition.type == type;
}

int64_t getEvaluation(
        Board *board, int depth, TranspositionTable *transpositions, int &nodeType, int64_t alpha, int64_t beta,
        const DeepEvaluationStrategy::Base *const furtherEvaluationStrategy = DeepEvaluationStrategy::Sequential::Instance
) {
    auto boardHash = board->getZobristHash();

    TranspositionTable::const_accessor accessor;
    auto isFound = transpositions->find(accessor, boardHash);

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

        auto evaluation = -MoveGenerator::deepEvaluate(board, depth - 1, furtherEvaluationStrategy, transpositions, -beta, -alpha);

        if (shouldUpdateTransposition(depth, nodeType, transposition))
            accessor->second->exchange({boardHash, evaluation, depth, nodeType});

        return evaluation;
    }

    auto evaluation = -MoveGenerator::deepEvaluate(board, depth - 1, furtherEvaluationStrategy, transpositions, -beta, -alpha);

    AtomicTranspositionPtr ptr(new std::atomic<Transposition>({boardHash, evaluation, depth, nodeType}));
    transpositions->insert({{boardHash, ptr}});

    return evaluation;
}

namespace DeepEvaluationStrategy {
    void Base::deepEvaluateMove(
            Board *board, MoveVariant move, int depth, TranspositionTable *transpositions, int &nodeType,
            int64_t &alpha, int64_t &beta, bool &shouldExit, EvaluationUpdateStrategy *strategy) const {
        board->makeMoveWithoutGeneratingMoves(move);
        auto evaluation = getEvaluation(board, depth, transpositions, nodeType, alpha, beta);
        board->unmakeMove(move);
        strategy->updateEvaluation(evaluation, shouldExit, alpha, beta, nodeType);
    }

    int64_t Base::deepEvaluate(Board *board, int depth, TranspositionTable *transpositions, int64_t alpha, int64_t beta) const {
        auto moves = std::vector(board->legalMoves);
        sortMoves(board, moves);
        auto nodeType = Transposition::UPPER;

        return _deepEvaluate(board, moves, depth, transpositions, nodeType, alpha, beta);
    };

    int64_t
    Sequential::_deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, TranspositionTable *transpositions, int &nodeType, int64_t alpha, int64_t beta) const {
        bool shouldExit = false;

        for (auto &move: moves) {
            if (shouldExit || analysisStopped) return alpha;
            deepEvaluateMove(board, move, depth, transpositions, nodeType, alpha, beta, shouldExit, strategy);
        }

        return alpha;
    }

    int64_t
    Parallel::_deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, TranspositionTable *transpositions, int &nodeType, int64_t alpha, int64_t beta) const {
        bool shouldExit = false;

        auto body = [this, board, moves, depth, &alpha, &beta, &shouldExit, transpositions, &nodeType](tbb::blocked_range<size_t> range) {
            for (size_t i = range.begin(); i < range.end(); ++i) {
                if (shouldExit || analysisStopped) return;
                auto boardCopy = board->copy();
                deepEvaluateMove(boardCopy, moves[i], depth, transpositions, nodeType, alpha, beta, shouldExit, strategy);
                delete boardCopy;
            }
        };

        tbb::parallel_for(tbb::blocked_range<size_t>(0, moves.size()), body);

        return alpha;
    }

    int64_t getNullWindowEval(Board *board, int depth, TranspositionTable *transpositions, int &nodeType, int64_t alpha) {
        return getEvaluation(board, depth, transpositions, nodeType, alpha, alpha + 1, Sequential::Instance);
    };

    int64_t Pvs::_deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, TranspositionTable *transpositions, int &nodeType, int64_t alpha,
                               int64_t beta) const {
        bool shouldExit = false;

        board->makeMoveWithoutGeneratingMoves(moves[0]);
        alpha = getEvaluation(board, depth, transpositions, nodeType, alpha, beta, Pvs::Instance);
        board->unmakeMove(moves[0]);

        for (int i = 1; i < moves.size(); i++) {
            board->makeMoveWithoutGeneratingMoves(moves[i]);
            auto nullWindowEval = getNullWindowEval(board, depth, transpositions, nodeType, alpha);
            board->unmakeMove(moves[i]);

            if (nullWindowEval != alpha) {
                // this move is better than the current option
                board->makeMoveWithoutGeneratingMoves(moves[i]);
                auto fullWindowEval = getEvaluation(board, depth, transpositions, nodeType, alpha, beta, Pvs::Instance);
                strategy->updateEvaluation(fullWindowEval, shouldExit, alpha, beta, nodeType);
                board->unmakeMove(moves[i]);
            }

            if (shouldExit || analysisStopped) return alpha;
        }

        return alpha;
    }

    int64_t
    ParallelPvs::_deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, TranspositionTable *transpositions,int &nodeType,  int64_t alpha,
                               int64_t beta) const {
        bool shouldExit = false;

        board->makeMoveWithoutGeneratingMoves(moves[0]);
        alpha = getEvaluation(board, depth, transpositions, nodeType, alpha, beta, getFirstMoveEvaluationStrategy());
        board->unmakeMove(moves[0]);

        tbb::parallel_for(tbb::blocked_range<size_t>(1, moves.size()), [moves, &nodeType, board, depth, transpositions, &shouldExit, this, &alpha, &beta](tbb::blocked_range<size_t> range) {
            for (int i = range.begin(); i < range.end(); i++) {
                auto initialAlpha = alpha;
                auto boardCopy = board->copy();
                auto moveCopy = moves[i];
                boardCopy->makeMoveWithoutGeneratingMoves(moveCopy);
                auto nullWindowEval = getNullWindowEval(boardCopy, depth, transpositions, nodeType, initialAlpha);
                boardCopy->unmakeMove(moveCopy);

                if (nullWindowEval != initialAlpha) {
                    // this move is better than the current option
                    boardCopy->makeMoveWithoutGeneratingMoves(moveCopy);
                    auto fullWindowEval = getEvaluation(boardCopy, depth, transpositions, nodeType, alpha, beta, Pvs::Instance);
                    strategy->updateEvaluation(fullWindowEval, shouldExit, alpha, beta, nodeType);
                    boardCopy->unmakeMove(moveCopy);
                }

                if (shouldExit || analysisStopped) return;
            }
        });

        return alpha;
    }

    const Base *const ParallelPvs::getFirstMoveEvaluationStrategy() const {
        return Pvs::Instance;
    }

    const Base *const ParallelPvsWithSequentialChildren::getFirstMoveEvaluationStrategy() const {
        return Pvs::Instance;
    }

    const Sequential *const Sequential::Instance = new Sequential();
    const Parallel *const Parallel::Instance = new Parallel();
    const Pvs *const Pvs::Instance = new Pvs();
    const ParallelPvs *const ParallelPvs::Instance = new ParallelPvs();
    const ParallelPvsWithSequentialChildren *const ParallelPvsWithSequentialChildren::Instance = new ParallelPvsWithSequentialChildren();
}
