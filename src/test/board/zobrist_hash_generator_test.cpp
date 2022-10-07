#include <gtest/gtest.h>
#include "../../main/board/zobrist_hash_generator.h"
#include "../../main/board/board_util.h"

TEST(ZobristHashGenerator, DifferentPositionsReturnDifferentHashes) {
    auto board = Board::fromFenString("rnbqkbnr/pppppppp/8/8/2B5/5Q2/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    auto otherBoard = Board::fromFenString("rnbqkbnr/pp1ppppp/2p5/3B4/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", Piece::Black);

    ASSERT_NE(ZobristHashGenerator.hash(board), ZobristHashGenerator.hash(otherBoard));
}

TEST(ZobristHashGenerator, TwoBoardsWithTheSamePositionAreReturnTheSameHash) {
    auto board = Board::fromFenString(Board::startPosition);
    auto otherBoard = Board::fromFenString(Board::startPosition);

    ASSERT_EQ(ZobristHashGenerator.hash(board), ZobristHashGenerator.hash(otherBoard));
}

TEST(ZobristHashGenerator, ConsidersCastlingRights) {
    auto board = Board::fromFenString(Board::startPosition);
    auto originalHash = ZobristHashGenerator.hash(board);
    board->castlingPieceMoved[Piece::White | Piece::King] = true;
    auto hashAfterFirstChange = ZobristHashGenerator.hash(board);

    ASSERT_NE(hashAfterFirstChange, originalHash);

    board->castlingPieceMoved[Piece::Black | Piece::RightRook] = true;

    ASSERT_NE(ZobristHashGenerator.hash(board), hashAfterFirstChange);
    ASSERT_NE(ZobristHashGenerator.hash(board), originalHash);
}

TEST(ZobristHashGenerator, BoardWhereEnPassantCanBeMade_HasADifferentHashTo_OneWhereNoEnPassantCanBeMade) {
    auto boardWithEnPassant = Board::fromFenString("rnbqkbnr/pppp1ppp/8/8/3pP3/8/PPP2PPP/RNBQKBNR w KQkq - 0 1");
    MoveVariant move = NormalMove::fromString("c2c4");
    boardWithEnPassant->makeMove(move);

    auto boardWithoutEnPassant = Board::fromFenString("rnbqkbnr/pppp1ppp/8/8/2PpP3/8/PP3PPP/RNBQKBNR w KQkq - 0 1", Piece::Black);

    ASSERT_NE(ZobristHashGenerator.hash(boardWithEnPassant), ZobristHashGenerator.hash(boardWithoutEnPassant));
}

TEST(ZobristHashGenerator, TwoDifferentBoardsWithEnPassantMoves_HaveDifferentHashes) {
    auto boardWithEnPassant = Board::fromFenString("rnbqkbnr/pppp1ppp/8/8/3pP3/8/PPP2PPP/RNBQKBNR w KQkq - 0 1");
    MoveVariant move = NormalMove::fromString("c2c4");
    boardWithEnPassant->makeMove(move);

    auto otherBoardWithEnPassant = Board::fromFenString("rnbqkbnr/pppp1ppp/8/8/2Pp4/8/PP2PPPP/RNBQKBNR w KQkq - 0 1");
    MoveVariant otherMove = NormalMove::fromString("e2e4");
    otherBoardWithEnPassant->makeMove(otherMove);

    ASSERT_NE(ZobristHashGenerator.hash(boardWithEnPassant), ZobristHashGenerator.hash(otherBoardWithEnPassant));
}

TEST(ZobristHashGenerator, TwoBoardsWithTheSamePositionButDifferentPlayerToMoveHaveDifferentHashes) {
    auto boardWhereWhiteMoves = Board::fromFenString(Board::startPosition, Piece::White);
    auto boardWhereBlackMoves = Board::fromFenString(Board::startPosition, Piece::Black);

    ASSERT_NE(ZobristHashGenerator.hash(boardWhereWhiteMoves), ZobristHashGenerator.hash(boardWhereBlackMoves));
}