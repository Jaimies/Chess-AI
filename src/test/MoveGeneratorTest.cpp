#include <gtest/gtest.h>
#include "../main/Board.h"
#include "../main/MoveGenerator.h"

TEST(MoveGeneratorTest, FindsBestMoveInPositionWithCheckmate) {
    auto board = Board::fromFenString("rnbqkbnr/pppppppp/8/8/2B5/5Q2/PPPPPPPP/RNBQKBNR w KQkq - 0 1", Piece::Black);
    auto move = MoveGenerator::getBestMove(board);

    ASSERT_TRUE(move->startSquare == 62 && move->targetSquare == 45
                || move->startSquare == 62 && move->targetSquare == 47
                || move->startSquare == 52 && move->targetSquare == 44);
}

TEST(MoveGeneratorTest, FindsBestMoveInPositionWithPotentialCaptures) {
    auto board = Board::fromFenString("rnbqkbnr/pp1ppppp/2p5/3B4/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", Piece::Black);
    auto move = MoveGenerator::getBestMove(board);

    std::cout << move->startSquare << " " << move->targetSquare << std::endl;
    ASSERT_TRUE(move->startSquare == 42 && move->targetSquare == 35);
}

TEST(MoveGeneratorTest, FindsCorrectBestMoveInPosition2) {
    Board *board = Board::fromFenString("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
    auto move = MoveGenerator::getBestMove(board);
    ASSERT_TRUE(move->startSquare == 25 && move->targetSquare == 29);
}

TEST(MoveGeneratorTest, UsesSensiblePositioning) {
    Board *board = Board::fromFenString(Board::startPosition);
    board->makeMove(NormalMove::fromString("e2e4"));
    auto move = MoveGenerator::getBestMove(board);
    std::cout << move->startSquare << " " << move->targetSquare;
}

TEST(MoveGeneratorTest, evaluationIncludesPositioning) {
    Board *boardWithBetterPosition = Board::fromFenString("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1");
    Board *boardWithBetterPositionButLessMaterial = Board::fromFenString("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/R1BQKBNR w KQkq - 0 1");
    Board *board = Board::fromFenString(Board::startPosition);

    std::cout << MoveGenerator::evaluate(boardWithBetterPosition, 0) << std::endl;
    std::cout << MoveGenerator::evaluate(boardWithBetterPositionButLessMaterial, 0) << std::endl;
    std::cout << MoveGenerator::evaluate(board, 0) << std::endl;
    EXPECT_TRUE(MoveGenerator::evaluate(boardWithBetterPosition, 0) > MoveGenerator::evaluate(board, 0));
}
