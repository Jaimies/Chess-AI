#pragma once

#include "../../board/board.h"
#include "../constants.h"
#include "../transposition_table.h"

class SequentialDeepEvaluationStrategy {
public:
    std::unordered_map<uint64_t, Transposition> transpositions;
    int64_t deepEvaluate(Board *board, int depth);
    int64_t alphaBeta(Board *board, int depth, int64_t alpha, int64_t beta);
    int64_t pureAlphaBeta(Board *board, int depth, int64_t alpha, int64_t beta);

    uint64_t positionsAnalysed = 0;
};