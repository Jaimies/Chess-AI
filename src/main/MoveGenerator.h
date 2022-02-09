#pragma once

namespace MoveGenerator {
    extern unsigned long positionsAnalyzed;

    Move *getBestMove(Board *board);
    long evaluate(Board *board);
    std::array<int64_t, 64> *&getSquareValueTable(Board *board, int piece);
}