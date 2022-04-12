#include <thread>
#include <mutex>
#include "../board/piece.h"
#include "../board/move.h"
#include "../board/zobrist_hash_generator.h"
#include <tbb/parallel_for.h>
#include <iostream>
#include "move_generator.h"
#include "../util/vector_util.h"
#include "constants.h"
#include "move_evaluation_data.h"
#include "deep_evaluation_strategy.h"
#include "transpositions.h"
#include "move_sorting.h"
#include "evaluation.h"
#include "../util/thread_util.h"

long MoveGenerator::searchCaptures(Board *board, long alpha, long beta) {
    auto evaluation = evaluate(board, 0);
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
MoveGenerator::deepEvaluate(Board *board, int depth, const DeepEvaluationStrategy::Base *strategy, TranspositionTable *transpositions, int64_t alpha, int64_t beta) {
    if (depth == 0) {
        positionsAnalyzed++;
        board->checkIfLegalMovesExist();
        return searchCaptures(board, alpha, beta);
    }

    board->generateMoves();

    if (board->legalMoves.empty()) {
        positionsAnalyzed++;
        return evaluatePositionWithoutMoves(board, depth);
    }

    return strategy->deepEvaluate(board, depth, transpositions, alpha, beta);
}

void MoveGenerator::evaluateMove(MoveEvaluationData *data, MoveVariant move, TranspositionTable *transpositions) {
    auto boardCopy = data->board->copy();
    boardCopy->makeMoveWithoutGeneratingMoves(move);
    auto evaluation = -deepEvaluate(boardCopy, data->depth, parallelPvsStrategy, transpositions);
    boardCopy->unmakeMove(move);

    data->mutex.lock();
    if (evaluation > data->bestEvaluation) {
        data->bestEvaluation = evaluation;
        data->bestMove = move;
    }
    data->mutex.unlock();

    delete boardCopy;
}

std::vector<MoveVariant> getSortedMoves(Board *board, Move *supposedBestMove) {
    auto moves = board->legalMoves;
    if (!supposedBestMove) {
        sortMoves(board, moves);
    } else {
        auto supposedBestMoveIndex = VectorUtil::indexOf(moves, supposedBestMove->toVariant());
        VectorUtil::move(moves, supposedBestMoveIndex, 0);
    }

    return moves;
}

int64_t MoveGenerator::getDeepEvaluation(Board *board, int depth, int64_t lowerBound, int64_t upperBound, TranspositionTable *transpositions) {
    return -deepEvaluate(board, depth, parallelStrategy, transpositions, -upperBound, -lowerBound);
}

int64_t MoveGenerator::getFirstMoveAlpha(Board *board, int depth, std::vector<MoveVariant> moves, TranspositionTable *transpositions) {
    board->makeMoveWithoutGeneratingMoves(moves[0]);
    int64_t firstMoveAlpha = -deepEvaluate(board, depth, parallelPvsStrategy, transpositions, minEvaluation, maxEvaluation);
    board->unmakeMove(moves[0]);
    return firstMoveAlpha;
}

Move *MoveGenerator::_getBestMove(Board *board, int depth, Move *supposedBestMove, AiSettings settings) {
    if (board->legalMoves.empty()) return nullptr;

    auto data = new MoveEvaluationData(board, depth);
    auto moves = getSortedMoves(board, supposedBestMove);

    auto transpositions = new TranspositionTable();
    int64_t alpha = getFirstMoveAlpha(board, depth, moves, transpositions);
    data->bestEvaluation = alpha;
    data->bestMove = moves[0];

    tbb::parallel_for(tbb::blocked_range<size_t>(1, moves.size()), [data, moves, depth, transpositions, board, &alpha, this](tbb::blocked_range<size_t> range) {
        Board * boardCopy = board->copy();
        for (size_t i = range.begin(); i < range.end(); i++) {
            auto initialAlpha = alpha;
            auto moveCopy = moves[i];
            boardCopy->makeMoveWithoutGeneratingMoves(moveCopy);
            auto eval = getDeepEvaluation(boardCopy, depth, initialAlpha, alpha + 1, transpositions);
            boardCopy->unmakeMove(moveCopy);
            if (eval != initialAlpha)  {
                evaluateMove(data, moves[i], transpositions);
                alpha = data->bestEvaluation;
                if (analysisFinished) return;
            };
            if (analysisFinished) return;
        }
        delete boardCopy;
    });

    deleteInTheBackground(transpositions);

    auto bestMove = data->bestMove.value();
    delete data;
    return visit(GetMovePointerVisitor, bestMove);
}

Move *MoveGenerator::getBestMove(Board *board, AiSettings settings) {
    using namespace std::chrono;

    steady_clock::time_point begin = steady_clock::now();
    Move *bestMove = nullptr;

    startThreadAndForget([&bestMove, board, settings, begin, this]() {
        int depth = 1;

        while (!analysisFinished) {
            Board *boardCopy = board->copy();
            auto supposedBestMove = _getBestMove(boardCopy, depth, bestMove, settings);
            delete boardCopy;
            if (analysisFinished) break;
            bestMove = supposedBestMove;
            auto millisCount = duration_cast<milliseconds>(steady_clock::now() - begin).count();
            analysisInfo = new AnalysisInfo{positionsAnalyzed, depth + 1, bestMove, millisCount};
            depth++;
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    while (!bestMove) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    analysisFinished = true;

    return bestMove;
}
