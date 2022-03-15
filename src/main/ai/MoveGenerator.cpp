#include <thread>
#include <mutex>
#include "../board/Piece.h"
#include "../board/Move.h"
#include "../board/zobrist_hash_generator.h"
#include <tbb/parallel_for.h>
#include "MoveGenerator.h"
#include "../util/VectorUtil.h"
#include "constants.h"
#include "move_evaluation_data.h"
#include "deep_evaluation_strategy.h"
#include "transpositions.h"
#include "move_sorting.h"
#include "evaluation.h"

unsigned long MoveGenerator::positionsAnalyzed = 0;
AnalysisInfo *MoveGenerator::analysisInfo = nullptr;

long searchCaptures(Board *board, long alpha, long beta) {
    auto evaluation = MoveGenerator::evaluate(board, 0);
    if (evaluation >= beta) return beta;

    alpha = std::max(alpha, evaluation);

    board->generateCaptures();
    auto moves = std::vector(board->legalMoves);
    sortMoves(board, moves);

    for (auto move: moves) {
        board->makeMoveWithoutGeneratingMoves(move);
        auto evaluation = -searchCaptures(board, -beta, -alpha);
        board->unmakeMove(move);

        if (evaluation >= beta) return beta;
        alpha = std::max(alpha, evaluation);
    }

    return alpha;
}

int64_t
MoveGenerator::deepEvaluate(Board *board, int depth, DeepEvaluationStrategy *strategy, TranspositionTable *transpositions, int64_t alpha, int64_t beta) {
    if (depth == 0) {
        MoveGenerator::positionsAnalyzed++;
        board->checkIfLegalMovesExist();
        return searchCaptures(board, alpha, beta);
    }

    board->generateMoves();

    if (board->legalMoves.empty()) {
        MoveGenerator::positionsAnalyzed++;
        return evaluatePositionWithoutMoves(board, depth);
    }

    return strategy->deepEvaluate(board, depth, transpositions, alpha, beta);
}

void evaluateMove(MoveEvaluationData *data, MoveVariant move, TranspositionTable *transpositions) {
    auto boardCopy = data->board->copy();
    boardCopy->makeMoveWithoutGeneratingMoves(move);
    auto evaluation = -MoveGenerator::deepEvaluate(boardCopy, data->depth, ParallelDeepEvaluationStrategy, transpositions);
    boardCopy->unmakeMove(move);

    data->mutex->lock();
    if (evaluation > *data->bestEvaluation) {
        *data->bestEvaluation = evaluation;
        data->bestMove = move;
    }
    data->mutex->unlock();

    delete boardCopy;
}

Move *_getBestMove(Board *board, int depth, AiSettings settings) {
    generateHashes();
    depthHashes.clear();
    auto transpositions = new TranspositionTable();

    for (int depthHashIndex = 0; depthHashIndex < depth; depthHashIndex++)
        depthHashes.push_back(get64rand());

    if (board->legalMoves.empty()) return nullptr;

    auto data = new MoveEvaluationData(board, depth);
    auto moves = board->legalMoves;

    tbb::parallel_for(tbb::blocked_range<size_t>(0, moves.size()), [data, moves, transpositions](tbb::blocked_range<size_t> range) {
        for (size_t i = range.begin(); i < range.end(); ++i) {
            if (analysisStopped) return;
            evaluateMove(data, moves[i], transpositions);
        }
    });

    // delete transpositions in the background to avoid delaying the analysis results
    new std::thread([transpositions]() { delete transpositions; });

    auto bestMove = data->bestMove.value();
    delete data;
    return visit(GetMovePointerVisitor, bestMove);
}

Move *MoveGenerator::getBestMove(Board *board, AiSettings settings) {
    using namespace std::chrono;

    analysisStopped = false;

    analysisInfo = nullptr;
    positionsAnalyzed = 0;
    Board *boardCopy = board->copy();
    steady_clock::time_point begin = steady_clock::now();
    Move *bestMove;

    new std::thread([&bestMove, boardCopy, settings, begin]() {
        int depth = 4;

        while (!analysisStopped) {
            bestMove = _getBestMove(boardCopy, depth, settings);
            auto millisCount = duration_cast<milliseconds>(steady_clock::now() - begin).count();
            analysisInfo = new AnalysisInfo{positionsAnalyzed, depth + 1, bestMove, millisCount};
            depth++;
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    while (!bestMove) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    analysisStopped = true;

    return bestMove;
}
