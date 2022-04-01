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
MoveGenerator::deepEvaluate(Board *board, int depth, const DeepEvaluationStrategy::Base *strategy, TranspositionTable *transpositions, int64_t alpha, int64_t beta) {
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
    auto evaluation = -MoveGenerator::deepEvaluate(boardCopy, data->depth, DeepEvaluationStrategy::Pvs::Instance, transpositions);
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

int64_t getDeepEvaluation(Board *board, int depth, int64_t lowerBound, int64_t upperBound, TranspositionTable *transpositions) {
    return -MoveGenerator::deepEvaluate(board, depth, DeepEvaluationStrategy::Sequential::Instance, transpositions, -upperBound, -lowerBound);
}

int64_t getFirstMoveAlpha(Board *board, int depth, std::vector<MoveVariant> moves, TranspositionTable *transpositions) {
    board->makeMoveWithoutGeneratingMoves(moves[0]);
    int64_t firstMoveAlpha = -MoveGenerator::deepEvaluate(board, depth, DeepEvaluationStrategy::Pvs::Instance, transpositions, minEvaluation, maxEvaluation);
    board->unmakeMove(moves[0]);
    return firstMoveAlpha;
}

Move *_getBestMove(Board *board, int depth, Move *supposedBestMove, AiSettings settings) {
    if (board->legalMoves.empty()) return nullptr;

    auto data = new MoveEvaluationData(board, depth);
    auto moves = getSortedMoves(board, supposedBestMove);

    auto transpositions = new TranspositionTable();
    int64_t firstMoveAlpha = getFirstMoveAlpha(board, depth, moves, transpositions);
    data->bestEvaluation = firstMoveAlpha;
    data->bestMove = moves[0];

    for (int i = 1; i < moves.size(); i++) {
        board->makeMoveWithoutGeneratingMoves(moves[i]);
        auto eval = getDeepEvaluation(board, depth, firstMoveAlpha, firstMoveAlpha + 1, transpositions);
        board->unmakeMove(moves[i]);

        if (eval != firstMoveAlpha) {
            evaluateMove(data, moves[i], transpositions);
            firstMoveAlpha = data->bestEvaluation;
        }

        if (analysisStopped) break;
    }
    deleteInTheBackground(transpositions);

    auto bestMove = data->bestMove.value();
    delete data;
    return visit(GetMovePointerVisitor, bestMove);
}

Move *MoveGenerator::getBestMove(Board *board, AiSettings settings) {
    using namespace std::chrono;

    analysisStopped = false;

    analysisInfo = nullptr;
    positionsAnalyzed = 0;
    steady_clock::time_point begin = steady_clock::now();
    Move *bestMove = nullptr;

    startThreadAndForget([&bestMove, board, settings, begin]() {
        int depth = 1;

        while (!analysisStopped) {
            Board *boardCopy = board->copy();
            auto supposedBestMove = _getBestMove(boardCopy, depth, bestMove, settings);
            delete boardCopy;
            if (analysisStopped) break;
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

    analysisStopped = true;

    return bestMove;
}
