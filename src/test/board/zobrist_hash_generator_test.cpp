#include <gtest/gtest.h>
#include "../../main/board/zobrist_hash_generator.h"

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