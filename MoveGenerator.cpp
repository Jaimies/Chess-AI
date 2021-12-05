#include <limits>
#include <algorithm>
#include "Piece.h"
#include "Move.h"

using Evaluation = long;

const Evaluation minEvaluation = std::numeric_limits<Evaluation>::min() + 10;
const Evaluation maxEvaluation = std::numeric_limits<Evaluation>::max() - 10;

namespace MoveGenerator {
    unsigned long positionsAnalyzed = 0;

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

    long deepEvaluate(
            Board *board, int depth = 3,
            long alpha = minEvaluation, long beta = maxEvaluation) {
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
            auto evaluation = -deepEvaluate(board, depth - 1, -beta, -alpha);
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

        if (board->legalMoves.empty()) return nullptr;

        long bestEvaluation = minEvaluation;
        Move *bestMove = nullptr;
        auto moves = board->legalMoves;

        for (auto move: moves) {
            board->makeMove(move);
            auto evaluation = -deepEvaluate(board);
            board->unmakeMove(move);

            if (evaluation >= bestEvaluation) {
                bestEvaluation = evaluation;
                bestMove = move;
            }
        }

        return bestMove;
    }

    Move *getBestMove(Board *board) {
        return _getBestMove(board->copy());
    }
}
