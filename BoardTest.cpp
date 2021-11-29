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

TEST(BoardTest, MoveCountIsCorrectInPosition5) {
    Board *board = Board::fromFenString("r3k2r/pb3p2/5npp/n2p4/1p1PPB2/6P1/P2N1PBP/R3K2R b KQkq -", Piece::Black);
    ASSERT_EQ(29, countMoves(board->copy(), 1));
    ASSERT_EQ(953, countMoves(board->copy(), 2));
    ASSERT_EQ(27990, countMoves(board->copy(), 3));
    ASSERT_EQ(909807, countMoves(board->copy(), 4));
}

TEST(BoardTest, MoveCountIsCorrectInBehtingPosition) {
    Board *board = Board::fromFenString("8/8/7p/3KNN1k/2p4p/8/3P2p1/8 w - - ; bm Kc6");
    ASSERT_EQ(25, countMoves(board->copy(), 1));
    ASSERT_EQ(180, countMoves(board->copy(), 2));
    ASSERT_EQ(4098, countMoves(board->copy(), 3));
    ASSERT_EQ(46270, countMoves(board->copy(), 4));
}

TEST(BoardTest, MoveCountIsCorrectInPositionWithPins) {
    Board *board = Board::fromFenString("8/Rbk5/8/8/8/8/8/2K5", Piece::Black);
    ASSERT_EQ(7, countMoves(board->copy(), 1));
    ASSERT_EQ(91, countMoves(board->copy(), 2));
    ASSERT_EQ(1060, countMoves(board->copy(), 3));
    ASSERT_EQ(18010, countMoves(board->copy(), 4));
}

TEST(BoardTest, MoveCountIsCorrectInPositionWherePawnCanCaptureAndPromote) {
    Board *board = Board::fromFenString("b7/1P6/8/3K3k/8/8/8/8 w - -");
    ASSERT_EQ(12, countMoves(board->copy(), 1));
    ASSERT_EQ(68, countMoves(board->copy(), 2));
    ASSERT_EQ(957, countMoves(board->copy(), 3));
    ASSERT_EQ(7428, countMoves(board->copy(), 4));
    ASSERT_EQ(116285, countMoves(board->copy(), 5));
}

TEST(BoardTest, DoesntAllowEnPassantWhereDoingSoExposesTheKing) {
    Board *board = Board::fromFenString("8/3p1p2/8/r1P1K1Pq/8/8/8/7k", Piece::Black);
    board->makeMove(new NormalMove(51, 35));

    ASSERT_EQ(7, countMoves(board->copy(), 1));
    ASSERT_EQ(184, countMoves(board->copy(), 2));
    ASSERT_EQ(997, countMoves(board->copy(), 3));
    ASSERT_EQ(30436, countMoves(board->copy(), 4));
}
