#include <thread>
#include <mutex>
#include "../board/Piece.h"
#include "../board/Move.h"
#include "../board/zobrist_hash_generator.h"
#include <tbb/parallel_for.h>
#include "MoveGenerator.h"
#include "square_value_tables.h"
#include "../util/VectorUtil.h"
#include "constants.h"
#include "move_evaluation_data.h"
#include "deep_evaluation_strategy.h"
#include "transpositions.h"
#include "move_sorting.h"

unsigned long MoveGenerator::positionsAnalyzed = 0;
AnalysisInfo *MoveGenerator::analysisInfo = nullptr;

Evaluation getPiecePositionValue(Board *board, int piece, int position) {
    auto squareValueTable = getSquareValueTable(board, piece);
    auto positionToAccess = Piece::getColour(piece) == Piece::White ? 63 - position : position;
    return (*squareValueTable)[positionToAccess] * 10;
}

int64_t performEvaluation(Board *board) {
    long sum = 0;

    for (int square = 0; square < 64; square++) {
        auto piece = board->squares[square];
        if (piece == Piece::None) continue;
        auto squareEvaluation = Piece::getValue(piece) + getPiecePositionValue(board, piece, square);
        if (Piece::getColour(piece) == board->colourToMove) sum += squareEvaluation;
        else sum -= squareEvaluation;
    }

    return sum;
}

Evaluation evaluatePositionWithoutMoves(Board *board, int depth) {
    // prefer to checkmate sooner, rather than later
    if (board->isKingUnderAttack) return checkmateEvaluation - depth;
    return 0;
}

long MoveGenerator::evaluate(Board *board, int depth) {
    if (!board->hasLegalMoves)
        return evaluatePositionWithoutMoves(board, depth);

    return performEvaluation(board);
}

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

int64_t MoveGenerator::deepEvaluate(Board *board, int depth, DeepEvaluationStrategy *strategy, int64_t alpha, int64_t beta) {
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

    return strategy->deepEvaluate(board, depth, alpha, beta);
}

void evaluateMove(MoveEvaluationData *data, MoveVariant move) {
    auto boardCopy = data->board->copy();
    boardCopy->makeMoveWithoutGeneratingMoves(move);
    auto evaluation = -MoveGenerator::deepEvaluate(boardCopy, data->depth, ParallelDeepEvaluationStrategy);
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
    transpositions->clear();

    for (int depthHashIndex = 0; depthHashIndex < depth; depthHashIndex++)
        depthHashes.push_back(get64rand());

    if (board->legalMoves.empty()) return nullptr;

    auto data = new MoveEvaluationData(board, depth);
    auto moves = board->legalMoves;
    std::vector<std::thread *> threads;

    tbb::parallel_for(tbb::blocked_range<size_t>(0, moves.size()), [data, moves](tbb::blocked_range<size_t> range) {
        for (size_t i = range.begin(); i < range.end(); ++i)
            evaluateMove(data, moves[i]);
    });

    auto bestMove = data->bestMove.value();
    delete data;
    return visit(GetMovePointerVisitor, bestMove);
}

Move *MoveGenerator::getBestMove(Board *board, AiSettings settings) {
    using namespace std::chrono;

    analysisInfo = nullptr;
    positionsAnalyzed = 0;
    Board *boardCopy = board->copy();
    steady_clock::time_point begin = steady_clock::now();
    int depth = 4;

    while (true) {
        auto bestMove = _getBestMove(boardCopy, depth, settings);
        auto millisCount = duration_cast<milliseconds>(steady_clock::now() - begin).count();

        if (millisCount > 2000) {
            delete boardCopy;
            analysisInfo = new AnalysisInfo{positionsAnalyzed, depth, bestMove, millisCount};
            return bestMove;
        }

        depth++;
    }
}