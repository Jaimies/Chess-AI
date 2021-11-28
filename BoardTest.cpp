#include "gtest/gtest.h"
#include "Board.cpp"

int countMoves(Board *board, int depth) {
    if (depth == 0) return 1;

    std::vector<Move *> moves = board->legalMoves;
    auto positionsCount = 0;

    for (auto move: moves) {
        board->makeMove(move);
        positionsCount += countMoves(board, depth - 1);
        board->unmakeMove(move);
    }

    return positionsCount;
}

TEST(BoardTest, MoveCountIsCorrectInStartPosition) {
    Board *board = Board::fromFenString(Board::startPosition);
    ASSERT_EQ(20, countMoves(board->copy(), 1));
    ASSERT_EQ(400, countMoves(board->copy(), 2));
    ASSERT_EQ(8902, countMoves(board->copy(), 3));
    ASSERT_EQ(197281, countMoves(board->copy(), 4));
}
