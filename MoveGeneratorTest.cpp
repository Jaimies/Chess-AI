#include <gtest/gtest.h>
#include "Board.cpp"
#include "MoveGenerator.cpp"

TEST(MoveGeneratorTest, FindsBestMoveInPositionWithCheckmate) {
    auto board = Board::fromFenString("rnbqkbnr/pppppppp/8/8/2B5/5Q2/PPPPPPPP/RNBQKBNR w KQkq - 0 1", Piece::Black);
    auto move = MoveGenerator::getBestMove(board);

    ASSERT_TRUE((move->startSquare == 62 && move->targetSquare == 45)
                || (move->startSquare == 52 && move->targetSquare == 44));
}

TEST(MoveGeneratorTest, FindsBestMoveInPositionWithPotentialCaptures) {
    auto board = Board::fromFenString("rnbqkbnr/pp1ppppp/2p5/3B4/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", Piece::Black);
    auto move = MoveGenerator::getBestMove(board);

    ASSERT_TRUE(move->startSquare == 42 && move->targetSquare == 35);
}
