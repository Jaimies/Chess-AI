#include <limits>
#include <thread>
#include <mutex>
#include "../board/Piece.h"
#include "../board/Move.h"
#include "../board/zobrist_hash_generator.h"
#include <tbb/concurrent_hash_map.h>
#include "MoveGenerator.h"
#include "square_value_tables.h"
#include "../util/VectorUtil.h"

using Evaluation = long;

const Evaluation minEvaluation = std::numeric_limits<Evaluation>::min() + 10;
const Evaluation maxEvaluation = std::numeric_limits<Evaluation>::max() - 10;

const Evaluation checkmateEvaluation = minEvaluation + 1000;

typedef tbb::concurrent_hash_map<uint64_t, int64_t> TranspositionTable;

unsigned long MoveGenerator::positionsAnalyzed = 0;
AnalysisInfo *MoveGenerator::analysisInfo = nullptr;
std::vector<uint64_t> depthHashes;
auto transpositions = new TranspositionTable();

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

auto determineIfMoveCanCaptureVisitor = DetermineIfMoveCanCaptureVisitor();
auto getMoveAddedValueVisitor = GetMoveAddedValueVisitor();
auto getBasicMoveVisitor = GetBasicMoveVisitor();

int guessMoveValue(const Board *board, MoveVariant &move) {
    auto canCapture = visit(determineIfMoveCanCaptureVisitor, move);
    auto addedValue = visit(getMoveAddedValueVisitor, move);
    auto basicMove = visit(getBasicMoveVisitor, move);
    auto movePieceType = Piece::getType(board->squares[basicMove.startSquare]);
    auto capturePieceType = canCapture ? Piece::getType(board->squares[basicMove.targetSquare]) : Piece::None;

    int moveScoreGuess = 10 * (Piece::getValue(capturePieceType) + addedValue) - Piece::getValue(movePieceType);

    return moveScoreGuess;
}

void sortMoves(Board *board, std::vector<MoveVariant> &moves) {
    std::sort(moves.begin(), moves.end(), [board](MoveVariant &move, MoveVariant otherMove) {
        return guessMoveValue(board, move) > guessMoveValue(board, otherMove);
    });
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

int64_t deepEvaluate(
        Board *board, int depth,
        int64_t alpha = minEvaluation, int64_t beta = maxEvaluation) {
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

    TranspositionTable::const_accessor accessor;
    auto moves = std::vector(board->legalMoves);
    sortMoves(board, moves);

    for (auto move: moves) {
        board->makeMoveWithoutGeneratingMoves(move);

        auto boardHash = board->getZobristHash() ^ depthHashes[depth - 1];

        auto isFound = transpositions->find(accessor, boardHash);

        auto evaluation = !isFound
                          ? -deepEvaluate(board, depth - 1, -beta, -alpha)
                          : accessor->second;

        if (!isFound)
            transpositions->insert({boardHash, evaluation});

        board->unmakeMove(move);

        if (evaluation >= beta) return beta;
        alpha = std::max(alpha, evaluation);
    }

    return alpha;
}

Move *_getBestMove(Board *board, int depth) {
    generateHashes();
    depthHashes.clear();
    transpositions->clear();

    for (int depthHashIndex = 0; depthHashIndex < depth; depthHashIndex++)
        depthHashes.push_back(get64rand());

    if (board->legalMoves.empty()) return nullptr;

    int64_t bestDeepEvaluation = minEvaluation;

    Move *bestMove = nullptr;
    auto moves = board->legalMoves;

    std::mutex mutex;
    std::vector<std::thread *> threads;

    for (const auto& move: moves) {
        threads.push_back(new std::thread([move, depth, &bestDeepEvaluation, &bestMove, &mutex](Board *board) {
            auto moveCopy = move;
            board->makeMoveWithoutGeneratingMoves(moveCopy);
            auto deepEvaluation = -deepEvaluate(board, depth);
            board->unmakeMove(moveCopy);

            mutex.lock();
            if (deepEvaluation > bestDeepEvaluation) {
                bestDeepEvaluation = deepEvaluation;
                delete bestMove;
                bestMove = visit(GetMovePointerVisitor(), moveCopy);
            }
            mutex.unlock();

            delete board;
        }, board->copy()));
    }

    for (auto thread: threads) {
        thread->join();
        delete thread;
    }

    return bestMove;
}

Move *MoveGenerator::getBestMove(Board *board) {
    using namespace std::chrono;

    analysisInfo = nullptr;
    positionsAnalyzed = 0;
    Board *boardCopy = board->copy();
    steady_clock::time_point begin = steady_clock::now();
    int depth = 4;

    while (true) {
        auto bestMove = _getBestMove(boardCopy, depth);
        auto millisCount = duration_cast<milliseconds>(steady_clock::now() - begin).count();

        if (millisCount > 2000) {
            delete boardCopy;
            analysisInfo = new AnalysisInfo(positionsAnalyzed, depth, bestMove, millisCount);
            return bestMove;
        }

        depth++;
    }
}