#include <gtest/gtest.h>
#include "../../main/board/board_util.h"
#include "../../main/board/piece.h"
#include "../../main/board/board_squares.h"

TEST(BoardUtil, file) {
    ASSERT_EQ(BoardUtil::file(BoardSquares::a1), 0);
    ASSERT_EQ(BoardUtil::file(BoardSquares::f1), 5);
    ASSERT_EQ(BoardUtil::file(BoardSquares::h1), 7);
    ASSERT_EQ(BoardUtil::file(BoardSquares::a2), 0);
    ASSERT_EQ(BoardUtil::file(BoardSquares::h2), 7);
    ASSERT_EQ(BoardUtil::file(BoardSquares::a8), 0);
    ASSERT_EQ(BoardUtil::file(BoardSquares::h8), 7);
}

TEST(BoardUtil, rank) {
    ASSERT_EQ(BoardUtil::rank(BoardSquares::a1), 0);
    ASSERT_EQ(BoardUtil::rank(BoardSquares::h1), 0);
    ASSERT_EQ(BoardUtil::rank(BoardSquares::a2), 1);
    ASSERT_EQ(BoardUtil::rank(BoardSquares::h2), 1);
    ASSERT_EQ(BoardUtil::rank(BoardSquares::a3), 2);
    ASSERT_EQ(BoardUtil::rank(BoardSquares::b4), 3);
    ASSERT_EQ(BoardUtil::rank(BoardSquares::a5), 4);
    ASSERT_EQ(BoardUtil::rank(BoardSquares::a6), 5);
    ASSERT_EQ(BoardUtil::rank(BoardSquares::a7), 6);
    ASSERT_EQ(BoardUtil::rank(BoardSquares::a8), 7);
    ASSERT_EQ(BoardUtil::rank(BoardSquares::h8), 7);
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
    ASSERT_TRUE(BoardUtil::isPawnAtStartSquare(BoardSquares::a2, Piece::White | Piece::Pawn));
    ASSERT_TRUE(BoardUtil::isPawnAtStartSquare(BoardSquares::b2, Piece::White | Piece::Pawn));
    ASSERT_FALSE(BoardUtil::isPawnAtStartSquare(BoardSquares::a3, Piece::White | Piece::Pawn));
    ASSERT_TRUE(BoardUtil::isPawnAtStartSquare(BoardSquares::a7, Piece::Black | Piece::Pawn));
    ASSERT_TRUE(BoardUtil::isPawnAtStartSquare(BoardSquares::h7, Piece::Black | Piece::Pawn));
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
    ASSERT_TRUE(BoardUtil::isRookAtStartRank(BoardSquares::a1, Piece::White));
    ASSERT_TRUE(BoardUtil::isRookAtStartRank(BoardSquares::d1, Piece::White));
    ASSERT_TRUE(BoardUtil::isRookAtStartRank(BoardSquares::e1, Piece::White));
    ASSERT_TRUE(BoardUtil::isRookAtStartRank(BoardSquares::g1, Piece::White));
    ASSERT_TRUE(BoardUtil::isRookAtStartRank(BoardSquares::h1, Piece::White));
    ASSERT_FALSE(BoardUtil::isRookAtStartRank(BoardSquares::a2, Piece::White));
    ASSERT_FALSE(BoardUtil::isRookAtStartRank(BoardSquares::b4, Piece::White));

    ASSERT_FALSE(BoardUtil::isRookAtStartRank(BoardSquares::a1, Piece::Black));
    ASSERT_FALSE(BoardUtil::isRookAtStartRank(BoardSquares::e3, Piece::Black));
    ASSERT_TRUE(BoardUtil::isRookAtStartRank(BoardSquares::a8, Piece::Black));
    ASSERT_TRUE(BoardUtil::isRookAtStartRank(BoardSquares::d8, Piece::Black));
}

TEST(BoardUtil, isPawnAboutToPromote) {
    ASSERT_FALSE(BoardUtil::isPawnAboutToPromote(BoardSquares::a6, Piece::White | Piece::Pawn));
    ASSERT_TRUE(BoardUtil::isPawnAboutToPromote(BoardSquares::a7, Piece::White | Piece::Pawn));
    ASSERT_TRUE(BoardUtil::isPawnAboutToPromote(BoardSquares::b7, Piece::White | Piece::Pawn));

    ASSERT_FALSE(BoardUtil::isPawnAboutToPromote(BoardSquares::a7, Piece::Black | Piece::Pawn));
    ASSERT_TRUE(BoardUtil::isPawnAboutToPromote(BoardSquares::a2, Piece::Black | Piece::Pawn));
    ASSERT_TRUE(BoardUtil::isPawnAboutToPromote(BoardSquares::h2, Piece::Black | Piece::Pawn));
}
