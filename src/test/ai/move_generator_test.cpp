#include <gtest/gtest.h>
#include <thread>
#include "board/board.h"
#include "ai/move_generator.h"

TEST(MoveGenerator, FindsBestMoveInPositionWithCheckmate) {
    auto board = Board::fromFenString("rnbqkbnr/pppppppp/8/8/2B5/5Q2/PPPPPPPP/RNBQKBNR b KQkq - 0 1", Piece::Black);
    auto generator = new MoveGenerator();
    auto move = generator->getBestMove(board);

    std::cout << generator->analysisInfo->depthSearchedTo << std::endl;
    std::cout << generator->positionsAnalyzed << std::endl;
    std::cout << move->toString() << std::endl;

    ASSERT_TRUE(move->toString() == "g8f6"
                || move->toString() == "g8h6"
                || move->toString() == "e7e6");
}

TEST(MoveGenerator, FindsBestMoveInPositionWithPotentialCaptures) {
    auto board = Board::fromFenString("rnbqkbnr/pp1ppppp/2p5/3B4/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", Piece::Black);
    auto generator = new MoveGenerator();
    auto move = generator->getBestMove(board);

    std::cout << generator->analysisInfo->depthSearchedTo << std::endl;
    std::cout << generator->positionsAnalyzed << std::endl;

    ASSERT_EQ(move->toString(), "c6d5");
}

TEST(MoveGenerator, FindsCorrectBestMoveInPosition2) {
    Board *board = Board::fromFenString("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
    auto generator = new MoveGenerator();
    auto move = generator->getBestMove(board);

    std::cout << generator->analysisInfo->depthSearchedTo << std::endl;
    std::cout << generator->positionsAnalyzed << std::endl;

    ASSERT_EQ(move->toString(), "b4f4");
}

TEST(MoveGenerator, UsesSensiblePositioning) {
    Board *board = Board::fromFenString(Board::startPosition);
    MoveVariant moveToMake = NormalMove::fromString("e2e4");
    board->makeMove(moveToMake);
    auto generator = new MoveGenerator();
    auto move = generator->getBestMove(board);
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
    auto generator = new MoveGenerator();
    auto move = generator->getBestMove(board);

    std::cout << generator->positionsAnalyzed << std::endl;
    std::cout << generator->analysisInfo->depthSearchedTo << std::endl;

    EXPECT_EQ(move->toString(), "c3c1");
}

TEST(MoveGenerator, GivesCheckmateInAMoreComplexPosition) {
    auto board = Board::fromFenString("8/8/8/8/8/1k2r3/8/2K5 w - - 0 1", Piece::Black);
    auto generator = new MoveGenerator();
    auto move = generator->getBestMove(board);

    EXPECT_EQ(move->toString(), "e3d3");
}

TEST(MoveGenerator, DoesNotDoStupidMoves) {
    auto board = Board::fromFenString("r1b1kbnr/p1p2ppp/2p1p3/q2pP3/1P1P4/P1N5/2P2PPP/R1BQK1NR b - - 0 1", Piece::Black);

    for (int i = 0; i < 20; i++) {
        auto generator = new MoveGenerator();
        auto bestMove = generator->getBestMove(board);
        ASSERT_EQ(bestMove->toString(), "f8b4");
        std::cout << i + 1 << " / 20" << std::endl;
    }
}

TEST(MoveGenerator, DoesNotDoStupidMoves_2) {
    auto board = Board::fromFenString("r1b1kr2/ppp2ppp/1b2p3/P2pP3/1P1P4/2q2N2/2PN1PPP/R2QK2R b KQq - 0 1", Piece::Black);

    for (int i = 0; i < 50; i++) {
        auto generator = new MoveGenerator();
        auto bestMove = generator->getBestMove(board);
        ASSERT_EQ(bestMove->toString(), "b6d4");
        std::cout << i + 1 << " / 50" << std::endl;
    }
}

TEST(MoveGenerator, DoesNotDoStupidMoves_3) {
    auto board = Board::fromFenString("r1bqkb1r/pppppppp/2n5/8/2B1N3/8/PPPP1PPP/R1BQK2R b KQkq - 0 1", Piece::Black);

    for (int i = 0; i < 20; i++) {
        auto generator = new MoveGenerator();
        auto bestMove = generator->getBestMove(board);
        ASSERT_EQ(bestMove->toString(), "d7d5");
        std::cout << i + 1 << " / 20" << std::endl;
    }
}

TEST(MoveGenerator, DoesNotDoStupidMoves_4) {
    auto board = Board::fromFenString("r3kbnr/pppqpppp/2n5/1B3b2/3P4/2N2N2/PPP2PPP/R1BQK2R b KQk - 0 1", Piece::Black);

    for (int i = 0; i < 100; i++) {
        auto generator = new MoveGenerator();
        auto bestMove = generator->getBestMove(board);
        std::cout << bestMove->toString() << std::endl;
        std::cout << generator->analysisInfo->depthSearchedTo << std::endl;
        ASSERT_TRUE(bestMove->toString() == "g8f6" || bestMove->toString() == "a7a6");
        std::cout << i + 1 << " / 100" << std::endl;
    }
}
