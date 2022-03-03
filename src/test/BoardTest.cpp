#include "gtest/gtest.h"
#include "../main/board/Board.h"

int countMoves(Board *board, int depth) {
    if (depth == 0) return 1;

    board->generateMoves();
    std::vector<MoveVariant> moves = board->legalMoves;
    auto positionsCount = 0;

    for (auto move: moves) {
        board->makeMoveWithoutGeneratingMoves(move);
        positionsCount += countMoves(board, depth - 1);
        board->unmakeMove(move);
    }

    return positionsCount;
}

TEST(Board, MoveCountIsCorrectInStartPosition) {
    Board *board = Board::fromFenString(Board::startPosition);
    ASSERT_EQ(20, countMoves(board->copy(), 1));
    ASSERT_EQ(400, countMoves(board->copy(), 2));
    ASSERT_EQ(8902, countMoves(board->copy(), 3));
    ASSERT_EQ(197281, countMoves(board->copy(), 4));
}

TEST(Board, MoveCountIsCorrectInPosition2) {
    Board *board = Board::fromFenString("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");

    ASSERT_EQ(48, countMoves(board->copy(), 1));
    ASSERT_EQ(2039, countMoves(board->copy(), 2));
    ASSERT_EQ(97862, countMoves(board->copy(), 3));
    ASSERT_EQ(4085603, countMoves(board->copy(), 4));
    ASSERT_EQ(193690690, countMoves(board->copy(), 5));
}

TEST(Board, MoveCountIsCorrectInPosition3) {
    Board *board = Board::fromFenString("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");

    ASSERT_EQ(14, countMoves(board->copy(), 1));
    ASSERT_EQ(191, countMoves(board->copy(), 2));
    ASSERT_EQ(2812, countMoves(board->copy(), 3));
    ASSERT_EQ(43238, countMoves(board->copy(), 4));
    ASSERT_EQ(674624, countMoves(board->copy(), 5));
    ASSERT_EQ(11030083, countMoves(board->copy(), 6));
}

TEST(Board, MoveCountIsCorrectInPosition4) {
    Board *board = Board::fromFenString("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");

    ASSERT_EQ(6, countMoves(board->copy(), 1));
    ASSERT_EQ(264, countMoves(board->copy(), 2));
    ASSERT_EQ(9467, countMoves(board->copy(), 3));
    ASSERT_EQ(422333, countMoves(board->copy(), 4));
    ASSERT_EQ(15833292, countMoves(board->copy(), 5));
    ASSERT_EQ(706045033, countMoves(board->copy(), 6));
}

TEST(Board, MoveCountIsCorrectInPosition5) {
    Board *board = Board::fromFenString("r3k2r/pb3p2/5npp/n2p4/1p1PPB2/6P1/P2N1PBP/R3K2R b KQkq -", Piece::Black);
    ASSERT_EQ(29, countMoves(board->copy(), 1));
    ASSERT_EQ(953, countMoves(board->copy(), 2));
    ASSERT_EQ(27990, countMoves(board->copy(), 3));
    ASSERT_EQ(909807, countMoves(board->copy(), 4));
    ASSERT_EQ(26957954, countMoves(board->copy(), 5));
}

TEST(Board, MoveCountIsCorrectInAlternativePosition5) {
    Board *board = Board::fromFenString("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    ASSERT_EQ(44, countMoves(board->copy(), 1));
    ASSERT_EQ(1486, countMoves(board->copy(), 2));
    ASSERT_EQ(62379, countMoves(board->copy(), 3));
    ASSERT_EQ(2103487, countMoves(board->copy(), 4));
    ASSERT_EQ(89941194, countMoves(board->copy(), 5));
}

TEST(Board, MoveCountIsCorrectInPosition6) {
    Board *board = Board::fromFenString("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");

    ASSERT_EQ(46, countMoves(board->copy(), 1));
    ASSERT_EQ(2079, countMoves(board->copy(), 2));
    ASSERT_EQ(89890, countMoves(board->copy(), 3));
    ASSERT_EQ(3894594, countMoves(board->copy(), 4));
    ASSERT_EQ(164075551, countMoves(board->copy(), 5));
}

TEST(Board, MoveCountIsCorrectInPositionWherePawnCanCaptureAndPromote) {
    Board *board = Board::fromFenString("b7/1P6/8/3K3k/8/8/8/8 w - -");
    ASSERT_EQ(12, countMoves(board->copy(), 1));
    ASSERT_EQ(68, countMoves(board->copy(), 2));
    ASSERT_EQ(957, countMoves(board->copy(), 3));
    ASSERT_EQ(7428, countMoves(board->copy(), 4));
    ASSERT_EQ(116285, countMoves(board->copy(), 5));
}

TEST(Board, MoveCountIsCorrectInBehtingPosition) {
    Board *board = Board::fromFenString("8/8/7p/3KNN1k/2p4p/8/3P2p1/8 w - - ; bm Kc6");
    ASSERT_EQ(25, countMoves(board->copy(), 1));
    ASSERT_EQ(180, countMoves(board->copy(), 2));
    ASSERT_EQ(4098, countMoves(board->copy(), 3));
    ASSERT_EQ(46270, countMoves(board->copy(), 4));
}

TEST(Board, DoesntAllowEnPassantWhereDoingSoExposesTheKing) {
    Board *board = Board::fromFenString("8/3p1p2/8/r1P1K1Pq/8/8/8/7k", Piece::Black);
    MoveVariant move = NormalMove(51, 35);
    board->makeMove(move);

    ASSERT_EQ(7, countMoves(board->copy(), 1));
    ASSERT_EQ(184, countMoves(board->copy(), 2));
    ASSERT_EQ(997, countMoves(board->copy(), 3));
    ASSERT_EQ(30436, countMoves(board->copy(), 4));
}

TEST(Board, MoveCountIsCorrectInPositionWithPins) {
    Board *board = Board::fromFenString("8/Rbk5/8/8/8/8/8/2K5", Piece::Black);
    ASSERT_EQ(7, countMoves(board->copy(), 1));
    ASSERT_EQ(91, countMoves(board->copy(), 2));
    ASSERT_EQ(1060, countMoves(board->copy(), 3));
    ASSERT_EQ(18010, countMoves(board->copy(), 4));
}

TEST(Board, IsInEndgame) {
    ASSERT_TRUE(Board::fromFenString("q7/1k6/8/8/8/8/8/1Q2K3 w - - 0 1")->isInEndgame());
    ASSERT_TRUE(Board::fromFenString("q7/1k6/8/8/8/8/2B5/1Q2K3 w - - 0 1")->isInEndgame());
    ASSERT_FALSE(Board::fromFenString("q7/1k6/8/8/8/8/1NB5/1Q2K3 w - - 0 1")->isInEndgame());
    ASSERT_TRUE(Board::fromFenString("q7/1k1b4/8/8/8/8/2B5/1Q2K3 w - - 0 1")->isInEndgame());
    ASSERT_FALSE(Board::fromFenString("q7/1kr5/8/8/8/8/8/4K3 w - - 0 1")->isInEndgame());
}

TEST(Board, generateCaptureMoves) {
    auto board = Board::fromFenString("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
    board->generateCaptures();
    EXPECT_EQ(board->legalMoves.size(), 1);
    auto expectedMove = visit(GetMovePointerVisitor(), board->legalMoves[0]);
    EXPECT_EQ(expectedMove->startSquare, 25);
    EXPECT_EQ(expectedMove->targetSquare, 29);

    MoveVariant move = NormalMove::fromString("b4c4");
    board->makeMoveWithoutGeneratingMoves(move);

    board->generateCaptures();
    auto expectedMove2 = visit(GetMovePointerVisitor(), board->legalMoves[0]);
    EXPECT_EQ(board->legalMoves.size(), 1);
    EXPECT_EQ(expectedMove2->startSquare, 39);
    EXPECT_EQ(expectedMove2->targetSquare, 33);
}
