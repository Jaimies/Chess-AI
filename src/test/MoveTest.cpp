#include <gtest/gtest.h>
#include "../main/Piece.h"
#include "../main/Move.h"

TEST(MoveTest, FromString) {
    auto move = NormalMove::fromString("a1b1");
    EXPECT_EQ(move.startSquare, 0);
    EXPECT_EQ(move.targetSquare, 1);

    auto move2 = NormalMove::fromString("d2d4");
    EXPECT_EQ(move2.startSquare, 11);
    EXPECT_EQ(move2.targetSquare, 27);

    auto move3 = NormalMove::fromString("d7e5");
    EXPECT_EQ(move3.startSquare, 51);
    EXPECT_EQ(move3.targetSquare, 36);
}

TEST(MoveTest, ToString) {
    EXPECT_EQ(NormalMove::fromString("a1b1").toString(), "a1b1");
    EXPECT_EQ(NormalMove::fromString("d2d4").toString(), "d2d4");
    EXPECT_EQ(NormalMove::fromString("d7e5").toString(), "d7e5");
}
