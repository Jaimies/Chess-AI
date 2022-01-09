#include <gtest/gtest.h>
#include "zobrist_hash_generator.h"
#include "Board.h"

TEST(ZobristHashGeneratorTest, DifferentPositionsReturnDifferentHashes) {
    generateHashes();
    auto board = Board::fromFenString("rnbqkbnr/pppppppp/8/8/2B5/5Q2/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    auto otherBoard = Board::fromFenString("rnbqkbnr/pp1ppppp/2p5/3B4/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", Piece::Black);

    ASSERT_FALSE(hash(board) == hash(otherBoard));
}

TEST(ZobristHashGeneratorTest, TwoBoardsWithTheSamePositionAreReturnTheSameHash) {
    generateHashes();

    auto board = Board::fromFenString(Board::startPosition);
    auto otherBoard = Board::fromFenString(Board::startPosition);

    ASSERT_EQ(hash(board), hash(otherBoard));
}