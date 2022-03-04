#include <gtest/gtest.h>
#include "../main/board/Board.h"
#include "../main/ai/MoveGenerator.h"

TEST(MoveGenerator, FindsBestMoveInPositionWithCheckmate) {
    auto board = Board::fromFenString("rnbqkbnr/pppppppp/8/8/2B5/5Q2/PPPPPPPP/RNBQKBNR b KQkq - 0 1", Piece::Black);
    auto move = MoveGenerator::getBestMove(board);

    std::cout << MoveGenerator::analysisInfo->depthSearchedTo << std::endl;
    std::cout << MoveGenerator::positionsAnalyzed << std::endl;
    std::cout << move->toString() << std::endl;

    ASSERT_TRUE(move->startSquare == 62 && move->targetSquare == 45
                || move->startSquare == 62 && move->targetSquare == 47
                || move->startSquare == 52 && move->targetSquare == 44);
}

TEST(MoveGenerator, FindsBestMoveInPositionWithPotentialCaptures) {
    auto board = Board::fromFenString("rnbqkbnr/pp1ppppp/2p5/3B4/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", Piece::Black);
    auto move = MoveGenerator::getBestMove(board);

    std::cout << MoveGenerator::analysisInfo->depthSearchedTo << std::endl;
    std::cout << MoveGenerator::positionsAnalyzed << std::endl;

    ASSERT_TRUE(move->startSquare == 42 && move->targetSquare == 35);
}

TEST(MoveGenerator, FindsCorrectBestMoveInPosition2) {
    Board *board = Board::fromFenString("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
    auto move = MoveGenerator::getBestMove(board);

    std::cout << MoveGenerator::analysisInfo->depthSearchedTo << std::endl;
    std::cout << MoveGenerator::positionsAnalyzed << std::endl;

    ASSERT_TRUE(move->startSquare == 25 && move->targetSquare == 29);
}

TEST(MoveGenerator, UsesSensiblePositioning) {
    Board *board = Board::fromFenString(Board::startPosition);
    MoveVariant moveToMake = NormalMove::fromString("e2e4");
    board->makeMove(moveToMake);
    auto move = MoveGenerator::getBestMove(board);
    std::cout << move->startSquare << " " << move->targetSquare;
}

TEST(MoveGenerator, evaluationIncludesPositioning) {
    Board *boardWithBetterPosition = Board::fromFenString("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1");
    Board *boardWithBetterPositionButLessMaterial = Board::fromFenString("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/R1BQKBNR w KQkq - 0 1");
    Board *board = Board::fromFenString(Board::startPosition);

    EXPECT_TRUE(MoveGenerator::evaluate(boardWithBetterPosition, 0) > MoveGenerator::evaluate(board, 0));
    EXPECT_TRUE(MoveGenerator::evaluate(boardWithBetterPositionButLessMaterial, 0) < MoveGenerator::evaluate(board, 0));

    board->colourToMove = Piece::Black;
    boardWithBetterPositionButLessMaterial->colourToMove = Piece::Black;
    boardWithBetterPosition->colourToMove = Piece::Black;

    EXPECT_TRUE(MoveGenerator::evaluate(boardWithBetterPosition, 0) < MoveGenerator::evaluate(board, 0));
    EXPECT_TRUE(MoveGenerator::evaluate(boardWithBetterPositionButLessMaterial, 0) > MoveGenerator::evaluate(board, 0));
}

TEST(MoveGenerator, GivesCheckmate) {
    auto board = Board::fromFenString("8/8/8/8/8/1kr5/8/K7 w - - 0 1", Piece::Black);
    auto move = MoveGenerator::getBestMove(board);

    std::cout << MoveGenerator::positionsAnalyzed << std::endl;
    std::cout << MoveGenerator::analysisInfo->depthSearchedTo << std::endl;

    EXPECT_EQ(move->toString(), "c3c1");
}

TEST(MoveGenerator, GivesCheckmateInAMoreComplexPosition) {
    auto board = Board::fromFenString("8/8/8/8/8/1k2r3/8/2K5 w - - 0 1", Piece::Black);
    auto move = MoveGenerator::getBestMove(board);

    EXPECT_EQ(move->toString(), "e3d3");
}
