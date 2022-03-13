#include "deep_evaluation_strategy.h"
#include "MoveGenerator.h"

void deepEvaluateMove(
        Board *board, MoveVariant move, int depth, TranspositionTable *transpositions,
        int64_t &alpha, int64_t &beta, bool &shouldExit, EvaluationUpdateStrategy *strategy) {
    board->makeMoveWithoutGeneratingMoves(move);

    auto boardHash = board->getZobristHash() ^ depthHashes[depth - 1];

    TranspositionTable::const_accessor accessor;
    auto isFound = transpositions->find(accessor, boardHash);

    auto evaluation = !isFound
                      ? -MoveGenerator::deepEvaluate(board, depth - 1, SequentialDeepEvaluationStrategy, transpositions, -beta, -alpha)
                      : accessor->second;

    if (!isFound)
        transpositions->insert({boardHash, evaluation});

    board->unmakeMove(move);
    strategy->updateEvaluation(evaluation, shouldExit, alpha, beta);
}

_SequentialDeepEvaluationStrategy *SequentialDeepEvaluationStrategy = new _SequentialDeepEvaluationStrategy();
_ParallelDeepEvaluationStrategy *ParallelDeepEvaluationStrategy = new _ParallelDeepEvaluationStrategy();

int64_t DeepEvaluationStrategy::deepEvaluate(Board *board, int depth, TranspositionTable *transpositions, int64_t alpha, int64_t beta) {
    auto moves = std::vector(board->legalMoves);
    sortMoves(board, moves);

    return _deepEvaluate(board, moves, depth, transpositions, alpha, beta);
};

int64_t
_SequentialDeepEvaluationStrategy::_deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, TranspositionTable *transpositions, int64_t alpha, int64_t beta) {
    bool shouldExit = false;

    for (auto &move: moves) {
        if (shouldExit || analysisStopped) return alpha;
        deepEvaluateMove(board, move, depth, transpositions, alpha, beta, shouldExit, strategy);
    }

    return alpha;
}

int64_t
_ParallelDeepEvaluationStrategy::_deepEvaluate(Board *board, std::vector<MoveVariant> moves, int depth, TranspositionTable *transpositions, int64_t alpha, int64_t beta) {
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
