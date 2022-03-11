#pragma once

#include <mutex>
#include "../board/Board.h"
#include "constants.h"

struct MoveEvaluationData {
    const Board *board;
    const int depth;
    std::optional<MoveVariant> bestMove;
    int64_t *bestEvaluation = new int64_t(minEvaluation);
    std::mutex *mutex = new std::mutex;

    MoveEvaluationData(Board *board, int depth) : board(board), depth(depth) {}

    ~MoveEvaluationData() {
        delete mutex;
        delete bestEvaluation;
    }
};
