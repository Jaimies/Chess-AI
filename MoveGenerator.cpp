#include <limits>
#include <algorithm>
#include <thread>
#include <mutex>
#include "Piece.h"
#include "Move.h"
#include "zobrist_hash_generator.h"

using Evaluation = long;

const Evaluation minEvaluation = std::numeric_limits<Evaluation>::min() + 10;
const Evaluation maxEvaluation = std::numeric_limits<Evaluation>::max() - 10;

namespace MoveGenerator {
    unsigned long positionsAnalyzed = 0;
    std::unordered_map<uint64_t, long> transpositions;
    std::vector<uint64_t> depthHashes;

    std::vector<int> getPiecesOfColour(Board *board, int colour) {
        std::vector<int> result;

        for (auto square: board->squares) {
            if (Piece::getColour(square) == colour) result.push_back(square);
        }

        return result;
    }

    long evaluate(const std::vector<int> &pieces) {
        long sum = 0;

        for (auto piece: pieces) {
            sum += Piece::getValue(piece);
        }

        return sum;
    }

    long evaluate(Board *board) {
        if (!board->hasLegalMoves)
            return board->isKingUnderAttack ? minEvaluation : 0;

        auto evaluation = evaluate(getPiecesOfColour(board, board->colourToMove));
        auto opponentEvaluation = evaluate(getPiecesOfColour(board, Piece::getOpponentColour(board->colourToMove)));

        return evaluation - opponentEvaluation;
    }

    int guessMoveValue(const Board *board, Move *move) {
        auto movePieceType = Piece::getType(board->squares[move->startSquare]);
        auto capturePieceType = move->canCapture() ? Piece::getType(board->squares[move->targetSquare]) : Piece::None;

        int moveScoreGuess = 10 * Piece::getValue(capturePieceType) - Piece::getValue(movePieceType);

        if (auto *promotionMove = dynamic_cast<PromotionMove *>(move)) {
            moveScoreGuess += Piece::getValue(promotionMove->pieceToPromoteTo);
        }

        return moveScoreGuess;
    }

    void sortMoves(Board *board, std::vector<Move *> &moves) {
        std::sort(moves.begin(), moves.end(), [board](Move *move, Move *otherMove) {
            return guessMoveValue(board, move) > guessMoveValue(board, otherMove);
        });
    }

    long searchCaptures(Board *board, long alpha, long beta) {
        auto evaluation = evaluate(board);
        if (evaluation >= beta) return beta;

        alpha = std::max(alpha, evaluation);

        board->generateMoves(true);
        auto moves = std::vector(board->legalMoves);
        sortMoves(board, moves);

        for (int index = 0; index < moves.size(); index++) {
            auto move = moves[index];
            board->makeMoveWithoutGeneratingMoves(move);
            auto evaluation = -searchCaptures(board, -beta, -alpha);
            board->unmakeMove(move);

            delete move;

            if (evaluation >= beta) {
                index++;
                for (; index < moves.size(); index++) delete moves[index];
                return beta;
            }
            alpha = std::max(alpha, evaluation);
        }

        return alpha;
    }

    int64_t deepEvaluate(
            Board *board, int depth,
            int64_t alpha = minEvaluation, int64_t beta = maxEvaluation) {
        if (depth == 0) {
            positionsAnalyzed++;
            board->checkIfLegalMovesExist();
            return searchCaptures(board, alpha, beta);
        }

        board->generateMoves();

        if (board->legalMoves.empty()) {
            positionsAnalyzed++;
            return evaluate(board);
        }

        auto moves = std::vector(board->legalMoves);
        sortMoves(board, moves);

        for (int index = 0; index < moves.size(); index++) {
            auto move = moves[index];
            board->makeMoveWithoutGeneratingMoves(move);

            auto boardHash = hash(board) ^ depthHashes[depth - 1];
            auto cachedEvaluation = transpositions.find(boardHash);

            auto evaluation = cachedEvaluation == transpositions.end()
                              ? -deepEvaluate(board, depth - 1, -beta, -alpha)
                              : cachedEvaluation->second;

//            if (cachedEvaluation == transpositions.end())
//                transpositions.insert(std::make_pair(boardHash, evaluation));

            board->unmakeMove(move);

            delete move;

            if (evaluation >= beta) {
                index++;
                for (; index < moves.size(); index++) delete moves[index];
                return beta;
            }
            alpha = std::max(alpha, evaluation);
        }

        return alpha;
    }

    Move *_getBestMove(Board *board) {
        positionsAnalyzed = 0;
        int depth = 5;
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
            threads.push_back(new std::thread([move, depth, &bestEvaluation, &bestDeepEvaluation, &bestMove, &mutex](Board *board) {
                board->makeMove(move);
                auto deepEvaluation = -deepEvaluate(board, depth);
                auto evaluation = -evaluate(board);

                board->unmakeMove(move);

                if (deepEvaluation > bestDeepEvaluation || deepEvaluation == bestDeepEvaluation && evaluation > bestEvaluation) {
                    mutex.lock();
                    bestDeepEvaluation = deepEvaluation;
                    bestEvaluation = evaluation;
                    bestMove = move;
                    mutex.unlock();
                }
            }, board->copy()));
        }

        for(auto thread: threads) {
            thread->join();
            delete thread;
        }

        return bestMove;
    }

    Move *getBestMove(Board *board) {
        return _getBestMove(board->copy());
    }
}
