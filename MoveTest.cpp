#include <gtest/gtest.h>
#include "Piece.h"
#include "Move.h"

TEST(MoveTest, FromString) {
    auto move = NormalMove::fromString("a1b1");
    EXPECT_EQ(move->startSquare, 0);
    EXPECT_EQ(move->targetSquare, 1);

    auto move2 = NormalMove::fromString("d2d4");
    EXPECT_EQ(move2->startSquare, 11);
    EXPECT_EQ(move2->targetSquare, 27);

    auto move3 = NormalMove::fromString("d7e5");
    EXPECT_EQ(move3->startSquare, 51);
    EXPECT_EQ(move3->targetSquare, 36);
}

TEST(MoveTest, ToString) {
    std::vector<std::string> positions{
            "a1b1",
            "d2d4",
            "d7e5",
            "a5b4",
            "d1f2",
    };

    for (auto position: positions) {
        EXPECT_EQ(NormalMove::fromString(position)->toString(), position);
    }
}
