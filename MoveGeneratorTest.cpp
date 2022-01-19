#include <gtest/gtest.h>
#include "Board.h"
#include "MoveGenerator.cpp"

TEST(MoveGeneratorTest, FindsBestMoveInPositionWithCheckmate) {
    auto board = Board::fromFenString("rnbqkbnr/pppppppp/8/8/2B5/5Q2/PPPPPPPP/RNBQKBNR w KQkq - 0 1", Piece::Black);
    auto move = MoveGenerator::getBestMove(board);

    ASSERT_TRUE(move->startSquare == 62 && move->targetSquare == 45
                || move->startSquare == 52 && move->targetSquare == 44
                || move->startSquare == 53 && move->targetSquare == 45
                || move->startSquare == 62 && move->targetSquare == 47);
}

TEST(MoveGeneratorTest, FindsBestMoveInPositionWithPotentialCaptures) {
    auto board = Board::fromFenString("rnbqkbnr/pp1ppppp/2p5/3B4/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", Piece::Black);
    auto move = MoveGenerator::getBestMove(board);

    ASSERT_TRUE(move->startSquare == 42 && move->targetSquare == 35);
}

TEST(MoveGeneratorTest, FindsCorrectBestMoveInPosition2) {
    Board *board = Board::fromFenString("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
    auto move = MoveGenerator::getBestMove(board);
    ASSERT_TRUE(move->startSquare == 25 && move->targetSquare == 29);
}
