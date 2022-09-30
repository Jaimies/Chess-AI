#include <gtest/gtest.h>
#include "../../main/board/piece.h"
#include "../../main/move/move.h"
#include "../../main/board/board_squares.h"

TEST(Move, FromString) {
    auto move = NormalMove::fromString("a1b1");
    EXPECT_EQ(move.startSquare, BoardSquares::a1);
    EXPECT_EQ(move.targetSquare, BoardSquares::b1);

    auto move2 = NormalMove::fromString("d2d4");
    EXPECT_EQ(move2.startSquare, BoardSquares::d2);
    EXPECT_EQ(move2.targetSquare, BoardSquares::d4);

    auto move3 = NormalMove::fromString("d7e5");
    EXPECT_EQ(move3.startSquare, BoardSquares::d7);
    EXPECT_EQ(move3.targetSquare, BoardSquares::e5);
}

TEST(Move, ToString) {
    EXPECT_EQ(NormalMove::fromString("a1b1").toString(), "a1b1");
    EXPECT_EQ(NormalMove::fromString("d2d4").toString(), "d2d4");
    EXPECT_EQ(NormalMove::fromString("d7e5").toString(), "d7e5");
}
