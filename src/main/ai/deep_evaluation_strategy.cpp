#include "deep_evaluation_strategy.h"
#include "move_generator.h"
#include "transpositions.h"
#include "../board/zobrist_hash_generator.h"

int getEvaluationType(int64_t eval, int64_t &alpha, int64_t &beta) {
    if (eval <= alpha) return Transposition::UPPER;
    if (eval >= beta) return Transposition::LOWER;
    return Transposition::EXACT;
}

int64_t getEvaluation(
        Board *board, int depth, TranspositionTable *transpositions, int64_t alpha, int64_t beta,
        const DeepEvaluationStrategy::Base * const furtherEvaluationStrategy = DeepEvaluationStrategy::Sequential::Instance
) {
    auto boardHash = board->getZobristHash();

    TranspositionTable::const_accessor accessor;
    auto isFound = transpositions->find(accessor, boardHash);

    if (isFound && accessor->second.zobristKey == boardHash && accessor->second.depth >= depth) {
        if (accessor->second.type == Transposition::EXACT)
            return accessor->second.value;

        if (accessor->second.type == Transposition::LOWER && accessor->second.value >= beta)
            return beta;

        if (accessor->second.type == Transposition::UPPER && accessor->second.value <= alpha)
            return alpha;
    }

    auto evaluation = -MoveGenerator::deepEvaluate(board, depth - 1, furtherEvaluationStrategy, transpositions, -beta, -alpha);

    auto type = getEvaluationType(evaluation, alpha, beta);

    if (!isFound)
        transpositions->insert({boardHash, {boardHash, evaluation, depth, type}});

    return evaluation;
}

namespace DeepEvaluationStrategy {
    void Base::deepEvaluateMove(
            Board *board, MoveVariant move, int depth, TranspositionTable *transpositions,
            int64_t &alpha, int64_t &beta, bool &shouldExit, EvaluationUpdateStrategy *strategy) const {
        board->makeMoveWithoutGeneratingMoves(move);
        auto evaluation = getEvaluation(board, depth, transpositions, alpha, beta);
        board->unmakeMove(move);
        strategy->updateEvaluation(evaluation, shouldExit, alpha, beta);
    }

    int64_t Base::deepEvaluate(Board *board, int depth, TranspositionTable *transpositions, int64_t alpha, int64_t beta) const {
        auto moves = std::vector(board->legalMoves);
        sortMoves(board, moves);

        return _deepEvaluate(board, moves, depth, transpositions, alpha, beta);
    };

    int64_t
    Sequential::_deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, TranspositionTable *transpositions, int64_t alpha, int64_t beta) const {
        bool shouldExit = false;

        for (auto &move: moves) {
            if (shouldExit || analysisStopped) return alpha;
            deepEvaluateMove(board, move, depth, transpositions, alpha, beta, shouldExit, strategy);
        }

        return alpha;
    }

    int64_t
    Parallel::_deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, TranspositionTable *transpositions, int64_t alpha, int64_t beta) const {
        bool shouldExit = false;

        auto body = [this, board, moves, depth, &alpha, &beta, &shouldExit, transpositions](tbb::blocked_range<size_t> range) {
            for (size_t i = range.begin(); i < range.end(); ++i) {
                if (shouldExit || analysisStopped) return;
                auto boardCopy = board->copy();
                deepEvaluateMove(boardCopy, moves[i], depth, transpositions, alpha, beta, shouldExit, strategy);
                delete boardCopy;
            }
        };

        tbb::parallel_for(tbb::blocked_range<size_t>(0, moves.size()), body);

        return alpha;
    }

    const Sequential *const Sequential::Instance = new Sequential();
    const Parallel *const Parallel::Instance = new Parallel();
    const Pvs *const Pvs::Instance = new Pvs();

    int64_t getNullWindowEval(Board *board, int depth, TranspositionTable *transpositions, int64_t alpha) {
        return getEvaluation(board, depth, transpositions, alpha, alpha + 1, Sequential::Instance);
    };

    int64_t Pvs::_deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, TranspositionTable *transpositions, int64_t alpha,
                               int64_t beta) const {
        bool shouldExit = false;

        board->makeMoveWithoutGeneratingMoves(moves[0]);
        alpha = getEvaluation(board, depth, transpositions, alpha, beta, Pvs::Instance);
        board->unmakeMove(moves[0]);

        for (int i = 1; i < moves.size(); i++) {
            board->makeMoveWithoutGeneratingMoves(moves[i]);
            auto nullWindowEval = getNullWindowEval(board, depth, transpositions, alpha);
            board->unmakeMove(moves[i]);

            if (nullWindowEval != alpha) {
                // this move is better than the current option
                board->makeMoveWithoutGeneratingMoves(moves[i]);
                auto fullWindowEval = getEvaluation(board, depth, transpositions, alpha, beta, Pvs::Instance);
                strategy->updateEvaluation(fullWindowEval, shouldExit, alpha, beta);
                board->unmakeMove(moves[i]);
            }

            if (shouldExit || analysisStopped) return alpha;
        }

        return alpha;
    }
}
