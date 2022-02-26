#pragma once

namespace MoveGenerator {
    extern unsigned long positionsAnalyzed;
    extern int depthSearchedTo;

    Move *getBestMove(Board *board);
    long evaluate(Board *board, int depth);
    const std::array<int64_t, 64> *getSquareValueTable(Board *board, int piece);
}