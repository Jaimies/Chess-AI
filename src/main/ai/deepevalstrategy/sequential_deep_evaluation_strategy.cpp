#include <iostream>
#include "sequential_deep_evaluation_strategy.h"
#include "ai/evaluation.h"
#include "ai/search_captures.h"
#include "ai/move_sorting.h"

int64_t SequentialDeepEvaluationStrategy::deepEvaluate(Board *board, int depth) {
    if (depth == 0) {
        positionsAnalysed++;
        return searchCaptures(board, EvalValues::min, EvalValues::max);
    }

    board->generateMoves();

    if (board->legalMoves.empty()) {
        return evaluatePositionWithoutMoves(board, depth);
    }

    auto moves = std::vector(board->legalMoves);
    sortMoves(board, moves);

    int64_t alpha = EvalValues::min;

    for (auto move: moves) {
        board->makeMoveWithoutGeneratingMoves(move);
        auto hash = board->getZobristHash();
        auto iterator = transpositions.find(hash);

        if (iterator != transpositions.end()) {
            auto transposition = iterator->second;

            if (transposition.depth >= depth) {
                auto eval = transposition.value;
                if (eval > alpha) alpha = eval;
            } else {
                auto eval = -deepEvaluate(board, depth - 1);
                transpositions.insert({hash, {eval, depth, Transposition::EXACT}});
                if (eval > alpha) alpha = eval;
            }
        } else {
            auto eval = -deepEvaluate(board, depth - 1);
            transpositions.insert({hash, {eval, depth, Transposition::EXACT}});
            if (eval > alpha) alpha = eval;
        }

        board->unmakeMove(move);
    }

    return alpha;
}

static int getNodeType(int64_t eval, int64_t alpha, int64_t beta) {
    if (eval >= beta) return Transposition::LOWER;
    if (eval <= alpha) return Transposition::UPPER;
    if (eval > alpha && eval < beta) return Transposition::EXACT;
    std::cout << "retard" << std::endl;
    throw 23;
}

int64_t SequentialDeepEvaluationStrategy::alphaBeta(Board *board, int depth, int64_t alpha, int64_t beta) {
    if (depth == 0) {
        positionsAnalysed++;
        return searchCaptures(board, alpha, beta);
    }

    board->generateMoves();

    if (board->legalMoves.empty()) {
        return evaluatePositionWithoutMoves(board, depth);
    }

    auto moves = std::vector(board->legalMoves);
    sortMoves(board, moves);

    for (auto move: moves) {
        board->makeMoveWithoutGeneratingMoves(move);
        auto hash = board->getZobristHash();
        auto iterator = transpositions.find(hash);

        if (iterator != transpositions.end()) {
            auto transposition = iterator->second;

            if (transposition.depth >= depth) {
                if (transposition.type == Transposition::EXACT && depth == transposition.depth) {
                    if(transposition.value > alpha)
                        alpha = transposition.value;
                } else if (transposition.type == Transposition::LOWER && transposition.value >= beta) {
                    board->unmakeMove(move);
                    return beta;
                } else if (transposition.type == Transposition::UPPER && transposition.value <= alpha) {
                } else {
                    auto eval = -alphaBeta(board, depth - 1, -beta, -alpha);
                    if (eval > alpha) alpha = eval;
                }
            } else {
                auto eval = -alphaBeta(board, depth - 1, -beta, -alpha);
                if (eval > alpha) alpha = eval;
            }
        } else {
            auto eval = -alphaBeta(board, depth - 1, -beta, -alpha);
            transpositions.insert({hash, {eval, depth, getNodeType(eval, alpha, beta)}});
            if (eval > alpha) alpha = eval;
        }

        board->unmakeMove(move);

        if (alpha >= beta) return beta;
    }

    return alpha;
}

int64_t SequentialDeepEvaluationStrategy::pureAlphaBeta(Board *board, int depth, int64_t alpha, int64_t beta) {
    if (depth == 0) {
        positionsAnalysed++;
        return searchCaptures(board, alpha, beta);
    }

    board->generateMoves();

    if (board->legalMoves.empty()) {
        return evaluatePositionWithoutMoves(board, depth);
    }

    auto moves = std::vector(board->legalMoves);
    sortMoves(board, moves);

    for (auto move: moves) {
        board->makeMoveWithoutGeneratingMoves(move);
        auto eval = -pureAlphaBeta(board, depth - 1, -beta, -alpha);
        board->unmakeMove(move);
        if (eval > alpha) alpha = eval;
        if (alpha >= beta) return beta;
    }

    return alpha;
}
