#include <gtest/gtest.h>
#include "../../main/board/board_squares.h"

TEST(BoardSquares, fromString) {
    ASSERT_EQ(BoardSquares::fromString("a1"), BoardSquares::a1);
    ASSERT_EQ(BoardSquares::fromString("b1"), BoardSquares::b1);
    ASSERT_EQ(BoardSquares::fromString("c1"), BoardSquares::c1);
    ASSERT_EQ(BoardSquares::fromString("f1"), BoardSquares::f1);
    ASSERT_EQ(BoardSquares::fromString("a2"), BoardSquares::a2);
    ASSERT_EQ(BoardSquares::fromString("e2"), BoardSquares::e2);
    ASSERT_EQ(BoardSquares::fromString("d4"), BoardSquares::d4);
}

TEST(BoardSquares, toString) {
    ASSERT_EQ(BoardSquares::toString(BoardSquares::a1), "a1");
    ASSERT_EQ(BoardSquares::toString(BoardSquares::b1), "b1");
    ASSERT_EQ(BoardSquares::toString(BoardSquares::c1), "c1");
    ASSERT_EQ(BoardSquares::toString(BoardSquares::f1), "f1");
    ASSERT_EQ(BoardSquares::toString(BoardSquares::a2), "a2");
    ASSERT_EQ(BoardSquares::toString(BoardSquares::e2), "e2");
    ASSERT_EQ(BoardSquares::toString(BoardSquares::d4), "d4");
}
