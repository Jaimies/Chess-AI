#include <gtest/gtest.h>
#include "../main/Board.h"
#include "../main/MoveGenerator.cpp"

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

TEST(MoveGeneratorTest, getSquareValueTable) {
    auto boardInEndGame = Board::fromFenString("q7/1k6/8/8/8/8/8/1Q2K3 w - - 0 1");
    auto boardInMidGame = Board::fromFenString(Board::startPosition);

    ASSERT_EQ(MoveGenerator::getSquareValueTable(boardInMidGame, Piece::Pawn | Piece::White), pawnSquareValues);
    ASSERT_EQ(MoveGenerator::getSquareValueTable(boardInMidGame, Piece::Knight | Piece::White), knightSquareValues);
    ASSERT_EQ(MoveGenerator::getSquareValueTable(boardInMidGame, Piece::Bishop | Piece::White), bishopSquareValues);
    ASSERT_EQ(MoveGenerator::getSquareValueTable(boardInMidGame, Piece::Queen | Piece::White), queenSquareValues);
    ASSERT_EQ(MoveGenerator::getSquareValueTable(boardInMidGame, Piece::Rook | Piece::White), rookSquareValues);
    ASSERT_EQ(MoveGenerator::getSquareValueTable(boardInMidGame, Piece::King | Piece::White), kingMidGameSquareValues);
    ASSERT_EQ(MoveGenerator::getSquareValueTable(boardInEndGame, Piece::King | Piece::White), kingEndGameSquareValues);
}
