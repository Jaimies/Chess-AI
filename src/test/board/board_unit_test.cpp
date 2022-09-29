#include <gtest/gtest.h>
#include "../../main/board/board.h"
#include "../string_util.h"

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
    auto expectedMove = visit(GetMovePointerVisitor, board->legalMoves[0]);
    EXPECT_EQ(expectedMove->startSquare, 25);
    EXPECT_EQ(expectedMove->targetSquare, 29);

    MoveVariant move = NormalMove::fromString("b4c4");
    board->makeMoveWithoutGeneratingMoves(move);

    board->generateCaptures();
    auto expectedMove2 = visit(GetMovePointerVisitor, board->legalMoves[0]);
    EXPECT_EQ(board->legalMoves.size(), 1);
    EXPECT_EQ(expectedMove2->startSquare, 39);
    EXPECT_EQ(expectedMove2->targetSquare, 33);
}

void assertToFenStringWorksInPosition(std::string position) {
    EXPECT_EQ(Board::fromFenString(position)->toFenString(), takeUntil(position, ' '));
}

TEST(Board, toFenString) {
    assertToFenStringWorksInPosition(Board::startPosition);
    assertToFenStringWorksInPosition("r1b1kbnr/p1p2ppp/2p1p3/q2pP3/1P1P4/P1N5/2P2PPP/R1BQK1NR/ b - - 0 1");
    assertToFenStringWorksInPosition("8/2k5/3p4/p2P1p2/P2P1P2/4K3/8/8/ b - - 0 1");
}

TEST(Board, enPassantCapablePawnSquare_is_negativeOneIfThereAreNoEnPassantMoves) {
    auto board = Board::fromFenString("rnbqkbnr/pppp1ppp/8/8/3pP3/8/PPP2PPP/RNBQKBNR w KQkq - 0 1");
    MoveVariant move = NormalMove::fromString("c2c3");
    board->makeMove(move);
    ASSERT_EQ(board->enPassantCapablePawnSquare, -1);
}

TEST(Board, enPassantCapablePawnSquare_is_numOfSquareWhereThePawnStands) {
    auto board = Board::fromFenString("rnbqkbnr/pppp1ppp/8/8/3pP3/8/PPP2PPP/RNBQKBNR w KQkq - 0 1");
    MoveVariant move = NormalMove::fromString("c2c4");
    board->makeMove(move);
    ASSERT_EQ(board->enPassantCapablePawnSquare, 27);
}

TEST(Board, enPassantCapablePawnSquare_resetsToNegativeOneAfterAnotherMoveIsMade) {
    auto board = Board::fromFenString("rnbqkbnr/pppp1ppp/8/8/3pP3/8/PPP2PPP/RNBQKBNR w KQkq - 0 1");
    MoveVariant move = NormalMove::fromString("c2c4");
    board->makeMove(move);
    MoveVariant move2 = NormalMove::fromString("d7d5");
    board->makeMove(move2);
    ASSERT_EQ(board->enPassantCapablePawnSquare, -1);
}

TEST(Board, allCastlesArePossibleBeforeAnyCastlingPiecesMove) {
    auto board = Board::fromFenString("r3k2r/ppp1qppp/2np1n2/2b1p1B1/2B1P1b1/2NP1N2/PPP1QPPP/R3K2R w KQkq - 0 1");

    ASSERT_TRUE(board->canWhiteCastleLeft());
    ASSERT_TRUE(board->canWhiteCastleRight());
    ASSERT_TRUE(board->canBlackCastleLeft());
    ASSERT_TRUE(board->canBlackCastleRight());
}

TEST(Board, whiteCanNoLongerCastleInAnyDirectionIfTheirKingMoves) {
    auto board = Board::fromFenString("r3k2r/ppp1qppp/2np1n2/2b1p1B1/2B1P1b1/2NP1N2/PPP1QPPP/R3K2R w KQkq - 0 1");

    MoveVariant move = NormalMove::fromString("e1d1");
    board->makeMove(move);

    ASSERT_FALSE(board->canWhiteCastleLeft());
    ASSERT_FALSE(board->canWhiteCastleRight());
}

TEST(Board, whiteCanNoLongerCastleLeftIfTheirLeftRookMoves) {
    auto board = Board::fromFenString("r3k2r/ppp1qppp/2np1n2/2b1p1B1/2B1P1b1/2NP1N2/PPP1QPPP/R3K2R w KQkq - 0 1");

    MoveVariant move = NormalMove::fromString("a1c1");
    board->makeMove(move);

    ASSERT_FALSE(board->canWhiteCastleLeft());
    ASSERT_TRUE(board->canWhiteCastleRight());

}

TEST(Board, whiteCanNoLongerCastleRightIfTheirRightRookMoves) {
    auto board = Board::fromFenString("r3k2r/ppp1qppp/2np1n2/2b1p1B1/2B1P1b1/2NP1N2/PPP1QPPP/R3K2R w KQkq - 0 1");

    MoveVariant move = NormalMove::fromString("h1f1");
    board->makeMove(move);

    ASSERT_TRUE(board->canWhiteCastleLeft());
    ASSERT_FALSE(board->canWhiteCastleRight());
}

TEST(Board, blackCanNoLongerCastleInAnyDirectionIfTheirKingMoves) {
    auto board = Board::fromFenString("r3k2r/ppp1qppp/2np1n2/2b1p1B1/2B1P1b1/2NP1N2/PPP1QPPP/R3K2R w KQkq - 0 1", Piece::Black);

    MoveVariant move = NormalMove::fromString("e8d8");
    board->makeMove(move);

    ASSERT_FALSE(board->canBlackCastleLeft());
    ASSERT_FALSE(board->canBlackCastleRight());
}

TEST(Board, blackCanNoLongerCastleLeftIfTheirLeftRookMoves) {
    auto board = Board::fromFenString("r3k2r/ppp1qppp/2np1n2/2b1p1B1/2B1P1b1/2NP1N2/PPP1QPPP/R3K2R w KQkq - 0 1", Piece::Black);

    MoveVariant move = NormalMove::fromString("a8c8");
    board->makeMove(move);

    ASSERT_FALSE(board->canBlackCastleLeft());
    ASSERT_TRUE(board->canBlackCastleRight());

}

TEST(Board, blackCanNoLongerCastleRightIfTheirRightRookMoves) {
    auto board = Board::fromFenString("r3k2r/ppp1qppp/2np1n2/2b1p1B1/2B1P1b1/2NP1N2/PPP1QPPP/R3K2R w KQkq - 0 1", Piece::Black);

    MoveVariant move = NormalMove::fromString("h8f8");
    board->makeMove(move);

    ASSERT_TRUE(board->canBlackCastleLeft());
    ASSERT_FALSE(board->canBlackCastleRight());
}
