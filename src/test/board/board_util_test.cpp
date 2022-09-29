#include <gtest/gtest.h>
#include "../../main/board/board_util.h"
#include "../../main/board/piece.h"

TEST(BoardUtil, file) {
    ASSERT_EQ(BoardUtil::file(0), 0);
    ASSERT_EQ(BoardUtil::file(5), 5);
    ASSERT_EQ(BoardUtil::file(7), 7);
    ASSERT_EQ(BoardUtil::file(8), 0);
    ASSERT_EQ(BoardUtil::file(15), 7);
    ASSERT_EQ(BoardUtil::file(56), 0);
    ASSERT_EQ(BoardUtil::file(63), 7);
}

TEST(BoardUtil, rank) {
    ASSERT_EQ(BoardUtil::rank(0), 0);
    ASSERT_EQ(BoardUtil::rank(7), 0);
    ASSERT_EQ(BoardUtil::rank(8), 1);
    ASSERT_EQ(BoardUtil::rank(15), 1);
    ASSERT_EQ(BoardUtil::rank(16), 2);
    ASSERT_EQ(BoardUtil::rank(25), 3);
    ASSERT_EQ(BoardUtil::rank(32), 4);
    ASSERT_EQ(BoardUtil::rank(40), 5);
    ASSERT_EQ(BoardUtil::rank(48), 6);
    ASSERT_EQ(BoardUtil::rank(56), 7);
    ASSERT_EQ(BoardUtil::rank(63), 7);
}

TEST(BoardUtil, squareFromPosition) {
    ASSERT_EQ(BoardUtil::squareFromPosition("a1"), 0);
    ASSERT_EQ(BoardUtil::squareFromPosition("b1"), 1);
    ASSERT_EQ(BoardUtil::squareFromPosition("c1"), 2);
    ASSERT_EQ(BoardUtil::squareFromPosition("f1"), 5);
    ASSERT_EQ(BoardUtil::squareFromPosition("a2"), 8);
    ASSERT_EQ(BoardUtil::squareFromPosition("e2"), 12);
    ASSERT_EQ(BoardUtil::squareFromPosition("d4"), 27);
}

TEST(BoardUtil, initialRankOfPawn) {
    ASSERT_EQ(BoardUtil::initialRankOfPawn(Piece::White | Piece::Pawn), 1);
    ASSERT_EQ(BoardUtil::initialRankOfPawn(Piece::Black | Piece::Pawn), 6);
    ASSERT_EQ(BoardUtil::initialRankOfPawn(Piece::Black | Piece::Pawn), 6);
    ASSERT_THROW({ BoardUtil::initialRankOfPawn(0); }, std::invalid_argument);
    ASSERT_THROW({ BoardUtil::initialRankOfPawn(Piece::Pawn); }, std::invalid_argument);
}

TEST(BoardUtil, isPawnAtStartSquare) {
    ASSERT_TRUE(BoardUtil::isPawnAtStartSquare(8, Piece::White | Piece::Pawn));
    ASSERT_TRUE(BoardUtil::isPawnAtStartSquare(9, Piece::White | Piece::Pawn));
    ASSERT_FALSE(BoardUtil::isPawnAtStartSquare(16, Piece::White | Piece::Pawn));
    ASSERT_TRUE(BoardUtil::isPawnAtStartSquare(48, Piece::Black | Piece::Pawn));
    ASSERT_TRUE(BoardUtil::isPawnAtStartSquare(55, Piece::Black | Piece::Pawn));
}
