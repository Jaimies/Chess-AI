#include <gtest/gtest.h>
#include "../../main/board/board.h"
#include "../../main/board/zobrist_hash_generator.h"

void assertZobristHashIsTheSameAsFromZobristHashGenerator(Board *board, int depth) {
    board->generateMoves();
    std::vector<MoveVariant> moves = board->legalMoves;

    ASSERT_EQ(board->getZobristHash(), ZobristHashGenerator.hash(board));
    if (depth == 0) return;

    for (auto move: moves) {
        board->makeMoveWithoutGeneratingMoves(move);
        assertZobristHashIsTheSameAsFromZobristHashGenerator(board, depth - 1);
        board->unmakeMove(move);
    }
}

TEST(Board_Integrated, getZobristHash_isTheSameAs_ZobristHashGenerator_hash) {
    auto board = Board::fromFenString("r3kbnr/ppp1pppp/2n1q3/1B3b2/3P4/2N2N2/PPP2PPP/R1BQK2R b KQk - 0 1");
    assertZobristHashIsTheSameAsFromZobristHashGenerator(board, 5);
}
