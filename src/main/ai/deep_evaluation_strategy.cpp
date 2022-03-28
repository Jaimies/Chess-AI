#include "deep_evaluation_strategy.h"
#include "move_generator.h"
#include "transpositions.h"
#include "../board/zobrist_hash_generator.h"

int getEvaluationType(int64_t eval, int64_t &alpha, int64_t &beta) {
    if (eval <= alpha) return Transposition::UPPER;
    if (eval >= beta) return Transposition::LOWER;
    return Transposition::EXACT;
}

int64_t DeepEvaluationStrategyWithTranspositions::getEvaluation(Board *board, int depth, int64_t alpha, int64_t beta) {
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

    auto evaluation = -MoveGenerator::deepEvaluate(board, depth - 1, sequentialDeepEvaluationStrategy, -beta, -alpha);

    auto type = getEvaluationType(evaluation, alpha, beta);

    if (!isFound)
        transpositions->insert({boardHash, {boardHash, evaluation, depth, type}});

    return evaluation;
}

void DeepEvaluationStrategy::deepEvaluateMove(
        Board *board, MoveVariant move, int depth,
        int64_t &alpha, int64_t &beta, bool &shouldExit, EvaluationUpdateStrategy *strategy) {
    board->makeMoveWithoutGeneratingMoves(move);
    auto evaluation = getEvaluation(board, depth, alpha, beta);
    board->unmakeMove(move);
    strategy->updateEvaluation(evaluation, shouldExit, alpha, beta);
}

int64_t DeepEvaluationStrategy::deepEvaluate(Board *board, int depth, int64_t alpha, int64_t beta) {
    auto moves = std::vector(board->legalMoves);
    sortMoves(board, moves);

    return _deepEvaluate(board, moves, depth, alpha, beta);
};

int64_t
SequentialDeepEvaluationStrategy::_deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha, int64_t beta) {
    bool shouldExit = false;

    for (auto &move: moves) {
        if (shouldExit || analysisStopped) return alpha;
        deepEvaluateMove(board, move, depth, alpha, beta, shouldExit, strategy);
    }

    return alpha;
}

SequentialDeepEvaluationStrategy::SequentialDeepEvaluationStrategy(TranspositionTable *transpositions)
        : DeepEvaluationStrategyWithTranspositions(transpositions) {
    sequentialDeepEvaluationStrategy = this;
}

int64_t
ParallelDeepEvaluationStrategy::_deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, int64_t alpha, int64_t beta) {
    bool shouldExit = false;

    auto body = [this, board, moves, depth, &alpha, &beta, &shouldExit](tbb::blocked_range<size_t> range) {
        for (size_t i = range.begin(); i < range.end(); ++i) {
            if (shouldExit || analysisStopped) return;
            auto boardCopy = board->copy();
            deepEvaluateMove(boardCopy, moves[i], depth, alpha, beta, shouldExit, strategy);
            delete boardCopy;
        }
    };

    tbb::parallel_for(tbb::blocked_range<size_t>(0, moves.size()), body);

    return alpha;
}

ParallelDeepEvaluationStrategy::ParallelDeepEvaluationStrategy(TranspositionTable *transpositions)
        : DeepEvaluationStrategyWithTranspositions(transpositions) {
    sequentialDeepEvaluationStrategy = new SequentialDeepEvaluationStrategy(transpositions);
}
