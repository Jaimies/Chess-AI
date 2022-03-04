#include <gtest/gtest.h>
#include "../../main/board/zobrist_hash_generator.h"

TEST(ZobristHashGenerator, DifferentPositionsReturnDifferentHashes) {
    generateHashes();
    auto board = Board::fromFenString("rnbqkbnr/pppppppp/8/8/2B5/5Q2/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    auto otherBoard = Board::fromFenString("rnbqkbnr/pp1ppppp/2p5/3B4/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", Piece::Black);

    ASSERT_FALSE(hash(board) == hash(otherBoard));
}

TEST(ZobristHashGenerator, TwoBoardsWithTheSamePositionAreReturnTheSameHash) {
    generateHashes();

    auto board = Board::fromFenString(Board::startPosition);
    auto otherBoard = Board::fromFenString(Board::startPosition);

    ASSERT_EQ(hash(board), hash(otherBoard));
}

TEST(ZobristHashGenerator, ConsidersCastlingRights) {
    generateHashes();

    auto board = Board::fromFenString(Board::startPosition);
    auto originalHash = hash(board);
    board->castlingPieceMoved[Piece::White | Piece::King] = true;
    auto hashAfterFirstChange = hash(board);

    ASSERT_FALSE(hashAfterFirstChange == originalHash);

    board->castlingPieceMoved[Piece::Black | Piece::RightRook] = true;

    ASSERT_FALSE(hash(board) == hashAfterFirstChange);
    ASSERT_FALSE(hash(board) == originalHash);
}