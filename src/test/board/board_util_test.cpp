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
}

TEST(BoardUtil, isPawnAtStartSquare) {
    ASSERT_TRUE(BoardUtil::isPawnAtStartSquare(8, Piece::White | Piece::Pawn));
    ASSERT_TRUE(BoardUtil::isPawnAtStartSquare(9, Piece::White | Piece::Pawn));
    ASSERT_FALSE(BoardUtil::isPawnAtStartSquare(16, Piece::White | Piece::Pawn));
    ASSERT_TRUE(BoardUtil::isPawnAtStartSquare(48, Piece::Black | Piece::Pawn));
    ASSERT_TRUE(BoardUtil::isPawnAtStartSquare(55, Piece::Black | Piece::Pawn));
}

TEST(BoardUtil, isValidSquare) {
    ASSERT_FALSE(BoardUtil::isValidSquare(-1));
    ASSERT_FALSE(BoardUtil::isValidSquare(-3));
    ASSERT_TRUE(BoardUtil::isValidSquare(0));
    ASSERT_TRUE(BoardUtil::isValidSquare(6));
    ASSERT_FALSE(BoardUtil::isValidSquare(64));
    ASSERT_FALSE(BoardUtil::isValidSquare(65));
    ASSERT_FALSE(BoardUtil::isValidSquare(66));
}

TEST(BoardUtil, initialRankOfRook) {
    ASSERT_EQ(BoardUtil::initialRankOfRook(Piece::White), 0);
    ASSERT_EQ(BoardUtil::initialRankOfRook(Piece::Black), 7);
}

TEST(BoardUtil, isRookAtStartRank) {
    ASSERT_TRUE(BoardUtil::isRookAtStartRank(1, Piece::White));
    ASSERT_TRUE(BoardUtil::isRookAtStartRank(3, Piece::White));
    ASSERT_TRUE(BoardUtil::isRookAtStartRank(4, Piece::White));
    ASSERT_TRUE(BoardUtil::isRookAtStartRank(6, Piece::White));
    ASSERT_TRUE(BoardUtil::isRookAtStartRank(7, Piece::White));
    ASSERT_FALSE(BoardUtil::isRookAtStartRank(8, Piece::White));
    ASSERT_FALSE(BoardUtil::isRookAtStartRank(25, Piece::White));

    ASSERT_FALSE(BoardUtil::isRookAtStartRank(1, Piece::Black));
    ASSERT_FALSE(BoardUtil::isRookAtStartRank(20, Piece::Black));
    ASSERT_TRUE(BoardUtil::isRookAtStartRank(56, Piece::Black));
    ASSERT_TRUE(BoardUtil::isRookAtStartRank(59, Piece::Black));
}

TEST(BoardUtil, isPawnAboutToPromote) {
    ASSERT_FALSE(BoardUtil::isPawnAboutToPromote(40, Piece::White | Piece::Pawn));
    ASSERT_TRUE(BoardUtil::isPawnAboutToPromote(48, Piece::White | Piece::Pawn));
    ASSERT_TRUE(BoardUtil::isPawnAboutToPromote(49, Piece::White | Piece::Pawn));

    ASSERT_FALSE(BoardUtil::isPawnAboutToPromote(48, Piece::Black | Piece::Pawn));
    ASSERT_TRUE(BoardUtil::isPawnAboutToPromote(8, Piece::Black | Piece::Pawn));
    ASSERT_TRUE(BoardUtil::isPawnAboutToPromote(15, Piece::Black | Piece::Pawn));
}
