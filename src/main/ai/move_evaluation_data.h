#pragma once

#include <mutex>
#include "../board/Board.h"
#include "constants.h"

struct MoveEvaluationData {
    const Board *board;
    const int depth;
    std::optional<MoveVariant> bestMove;
    int64_t bestEvaluation = minEvaluation;
    std::mutex mutex;

    MoveEvaluationData(Board *board, int depth) : board(board), depth(depth) {}
};
