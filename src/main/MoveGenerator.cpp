#include <limits>
#include <thread>
#include <mutex>
#include "Piece.h"
#include "Move.h"
#include "zobrist_hash_generator.h"
#include <folly/concurrency/ConcurrentHashMap.h>
#include "MoveGenerator.h"
#include "VectorUtil.h"

using Evaluation = long;
using TranspositionTable = folly::ConcurrentHashMap<uint64_t, int64_t>;

const Evaluation minEvaluation = std::numeric_limits<Evaluation>::min() + 10;
const Evaluation maxEvaluation = std::numeric_limits<Evaluation>::max() - 10;

const Evaluation checkmateEvaluation = minEvaluation + 1000;

std::array<Evaluation, 64> *kingMidGameSquareValues = new std::array<Evaluation, 64>{
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -20, -30, -30, -40, -40, -30, -30, -20,
        -10, -20, -20, -20, -20, -20, -20, -10,
        20, 20, 0, 0, 0, 0, 20, 20,
        20, 30, 10, 0, 0, 10, 30, 20
};

std::array<Evaluation, 64> *kingEndGameSquareValues = new std::array<Evaluation, 64>{
        -50, -40, -30, -20, -20, -30, -40, -50,
        -30, -20, -10, 0, 0, -10, -20, -30,
        -30, -10, 20, 30, 30, 20, -10, -30,
        -30, -10, 30, 40, 40, 30, -10, -30,
        -30, -10, 30, 40, 40, 30, -10, -30,
        -30, -10, 20, 30, 30, 20, -10, -30,
        -30, -30, 0, 0, 0, 0, -30, -30,
        -50, -30, -30, -30, -30, -30, -30, -50
};

std::array<Evaluation, 64> *queenSquareValues = new std::array<Evaluation, 64>{
        -20, -10, -10, -5, -5, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 5, 5, 5, 0, -10,
        -5, 0, 5, 5, 5, 5, 0, -5,
        0, 0, 5, 5, 5, 5, 0, -5,
        -10, 5, 5, 5, 5, 5, 0, -10,
        -10, 0, 5, 0, 0, 0, 0, -10,
        -20, -10, -10, -5, -5, -10, -10, -20
};

std::array<Evaluation, 64> *bishopSquareValues = new std::array<Evaluation, 64>{
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 5, 5, 10, 10, 5, 5, -10,
        -10, 0, 10, 10, 10, 10, 0, -10,
        -10, 10, 10, 10, 10, 10, 10, -10,
        -10, 5, 0, 0, 0, 0, 5, -10,
        -20, -10, -10, -10, -10, -10, -10, -20,
};

std::array<Evaluation, 64> *knightSquareValues = new std::array<Evaluation, 64>{
        -50, -40, -30, -30, -30, -30, -40, -50,
        -40, -20, 0, 0, 0, 0, -20, -40,
        -30, 0, 10, 15, 15, 10, 0, -30,
        -30, 5, 15, 20, 20, 15, 5, -30,
        -30, 0, 15, 20, 20, 15, 0, -30,
        -30, 5, 10, 15, 15, 10, 5, -30,
        -40, -20, 0, 5, 5, 0, -20, -40,
        -50, -40, -30, -30, -30, -30, -40, -50,
};

std::array<Evaluation, 64> *rookSquareValues = new std::array<Evaluation, 64>{
        0, 0, 0, 0, 0, 0, 0, 0,
        5, 10, 10, 10, 10, 10, 10, 5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        0, 0, 0, 5, 5, 0, 0, 0
};

std::array<Evaluation, 64> *pawnSquareValues = new std::array<Evaluation, 64>{
        0, 0, 0, 0, 0, 0, 0, 0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5, 5, 10, 25, 25, 10, 5, 5,
        0, 0, 0, 20, 20, 0, 0, 0,
        5, -5, -10, 0, 0, -10, -5, 5,
        5, 10, 10, -20, -20, 10, 10, 5,
        0, 0, 0, 0, 0, 0, 0, 0
};

unsigned long MoveGenerator::positionsAnalyzed = 0;
int MoveGenerator::depthSearchedTo = 0;
folly::ConcurrentHashMap<uint64_t, int64_t> transpositions;
std::vector<uint64_t> depthHashes;

Evaluation getPiecePositionValue(Board *board, int piece, int position) {
    auto squareValueTable = MoveGenerator::getSquareValueTable(board, piece);
    auto positionToAccess = Piece::getColour(piece) == Piece::White ? 63 - position : position;
    return (*squareValueTable)[positionToAccess] * 10;
}

std::array<Evaluation, 64> *&MoveGenerator::getSquareValueTable(Board *board, int piece) {
    auto type = Piece::getType(piece);

    if (type == Piece::Pawn) return pawnSquareValues;
    if (type == Piece::Rook) return rookSquareValues;
    if (type == Piece::Bishop) return bishopSquareValues;
    if (type == Piece::Knight) return knightSquareValues;

    if (type == Piece::King) {
        if (board->isInEndgame()) return kingEndGameSquareValues;
        return kingMidGameSquareValues;
    }

    if (type == Piece::Queen) return queenSquareValues;

    throw std::invalid_argument("Expected a piece with a type, got " + std::to_string(piece));
}

long evaluateSide(Board *board, int color) {
    long sum = 0;

    for (int square = 0; square < 64; square++) {
        auto piece = board->squares[square];
        if (Piece::getColour(piece) != color) continue;
        sum += Piece::getValue(piece) + getPiecePositionValue(board, piece, square);
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

    auto evaluation = evaluateSide(board, board->colourToMove);
    auto opponentEvaluation = evaluateSide(board, Piece::getOpponentColour(board->colourToMove));

    return evaluation - opponentEvaluation;
}

auto determineIfMoveCanCaptureVisitor = DetermineIfMoveCanCaptureVisitor();
auto getMoveAddedValueVisitor = GetMoveAddedValueVisitor();
auto getBasicMoveVisitor = GetBasicMoveVisitor();

int guessMoveValue(const Board *board, MoveVariant move) {
    auto canCapture = visit(determineIfMoveCanCaptureVisitor, move);
    auto addedValue = visit(getMoveAddedValueVisitor, move);
    auto basicMove = visit(getBasicMoveVisitor, move);
    auto movePieceType = Piece::getType(board->squares[basicMove.startSquare]);
    auto capturePieceType = canCapture ? Piece::getType(board->squares[basicMove.targetSquare]) : Piece::None;

    int moveScoreGuess = 10 * (Piece::getValue(capturePieceType) + addedValue) - Piece::getValue(movePieceType);

    return moveScoreGuess;
}

void sortMoves(Board *board, std::vector<MoveVariant> &moves) {
    std::sort(moves.begin(), moves.end(), [board](MoveVariant move, MoveVariant otherMove) {
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

    for (int index = 0; index < moves.size(); index++) {
        auto move = moves[index];
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

    auto moves = std::vector(board->legalMoves);
    sortMoves(board, moves);

    for (int index = 0; index < moves.size(); index++) {
        auto move = moves[index];
        board->makeMoveWithoutGeneratingMoves(move);

        auto boardHash = board->getZobristHash() ^ depthHashes[depth - 1];
        auto cachedEvaluation = transpositions.find(boardHash);

        auto evaluation = cachedEvaluation == transpositions.end()
                          ? -deepEvaluate(board, depth - 1, -beta, -alpha)
                          : cachedEvaluation->second;

        if (cachedEvaluation == transpositions.end())
            transpositions.insert(std::pair(boardHash, evaluation));

        board->unmakeMove(move);

        if (evaluation >= beta) return beta;
        alpha = std::max(alpha, evaluation);
    }

    return alpha;
}

Move *_getBestMove(Board *board, int depth) {
    generateHashes();
    depthHashes.clear();
    transpositions.clear();

    for (int depthHashIndex = 0; depthHashIndex < depth; depthHashIndex++)
        depthHashes.push_back(get64rand());

    if (board->legalMoves.empty()) return nullptr;

    int64_t bestDeepEvaluation = minEvaluation;
    int64_t bestEvaluation = minEvaluation;

    Move *bestMove = nullptr;
    auto moves = board->legalMoves;

    std::mutex mutex;
    std::vector<std::thread *> threads;

    for (auto move: moves) {
//        threads.push_back(new std::thread([move, depth, &bestEvaluation, &bestDeepEvaluation, &bestMove, &mutex](Board *board) {
            board->makeMoveWithoutGeneratingMoves(move);
            auto deepEvaluation = -deepEvaluate(board, depth);
            auto evaluation = -MoveGenerator::evaluate(board, depth);

            board->unmakeMove(move);

            if (deepEvaluation > bestDeepEvaluation ||
                deepEvaluation == bestDeepEvaluation && evaluation > bestEvaluation) {
                mutex.lock();
                bestDeepEvaluation = deepEvaluation;
                bestEvaluation = evaluation;
                bestMove = visit(GetMovePointerVisitor(), move);
                mutex.unlock();
            }

//            delete board;
//        }, board->copy()));
    }

    for (auto thread: threads) {
        thread->join();
        delete thread;
    }

    return bestMove;
}

Move *MoveGenerator::getBestMove(Board *board) {
    using namespace std::chrono;

    positionsAnalyzed = 0;
    Board *boardCopy = board->copy();
    steady_clock::time_point begin = steady_clock::now();
    uint64_t depth = 1;

    while (true) {
        auto bestMove = _getBestMove(board->copy(), depth);
        auto millisCount = duration_cast<milliseconds>(steady_clock::now() - begin).count();

        if (millisCount > 2000) {
            delete boardCopy;
            depthSearchedTo = depth;
            return bestMove;
        }

        depth++;
    }
}