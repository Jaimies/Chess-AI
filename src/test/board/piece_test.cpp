#include <gtest/gtest.h>
#include "../../main/board/piece.h"

TEST(Piece, getColour) {
    ASSERT_EQ(Piece::getColour(Piece::White | Piece::Pawn), Piece::White);
    ASSERT_EQ(Piece::getColour(Piece::White | Piece::Rook), Piece::White);
    ASSERT_EQ(Piece::getColour(Piece::White | Piece::Knight), Piece::White);
    ASSERT_EQ(Piece::getColour(Piece::White | Piece::Bishop), Piece::White);
    ASSERT_EQ(Piece::getColour(Piece::White | Piece::Queen), Piece::White);
    ASSERT_EQ(Piece::getColour(Piece::White | Piece::King), Piece::White);

    ASSERT_EQ(Piece::getColour(Piece::Black | Piece::Pawn), Piece::Black);
    ASSERT_EQ(Piece::getColour(Piece::Black | Piece::Rook), Piece::Black);
    ASSERT_EQ(Piece::getColour(Piece::Black | Piece::Knight), Piece::Black);
    ASSERT_EQ(Piece::getColour(Piece::Black | Piece::Bishop), Piece::Black);
    ASSERT_EQ(Piece::getColour(Piece::Black | Piece::Queen), Piece::Black);
    ASSERT_EQ(Piece::getColour(Piece::Black | Piece::King), Piece::Black);
}

TEST(Piece, getOpponentColour) {
    ASSERT_EQ(Piece::getOpponentColour(Piece::White), Piece::Black);
    ASSERT_EQ(Piece::getOpponentColour(Piece::Black), Piece::White);
}

TEST(Piece, getOpponentColourFromPiece) {
    ASSERT_EQ(Piece::getOpponentColourFromPiece(Piece::White | Piece::Pawn), Piece::Black);
    ASSERT_EQ(Piece::getOpponentColourFromPiece(Piece::White | Piece::Rook), Piece::Black);
    ASSERT_EQ(Piece::getOpponentColourFromPiece(Piece::White | Piece::Knight), Piece::Black);
    ASSERT_EQ(Piece::getOpponentColourFromPiece(Piece::White | Piece::Bishop), Piece::Black);
    ASSERT_EQ(Piece::getOpponentColourFromPiece(Piece::White | Piece::Queen), Piece::Black);
    ASSERT_EQ(Piece::getOpponentColourFromPiece(Piece::White | Piece::King), Piece::Black);

    ASSERT_EQ(Piece::getOpponentColourFromPiece(Piece::Black | Piece::Pawn), Piece::White);
    ASSERT_EQ(Piece::getOpponentColourFromPiece(Piece::Black | Piece::Rook), Piece::White);
    ASSERT_EQ(Piece::getOpponentColourFromPiece(Piece::Black | Piece::Knight), Piece::White);
    ASSERT_EQ(Piece::getOpponentColourFromPiece(Piece::Black | Piece::Bishop), Piece::White);
    ASSERT_EQ(Piece::getOpponentColourFromPiece(Piece::Black | Piece::Queen), Piece::White);
    ASSERT_EQ(Piece::getOpponentColourFromPiece(Piece::Black | Piece::King), Piece::White);
}

TEST(Piece, getType) {
    ASSERT_EQ(Piece::getType(Piece::White | Piece::Pawn), Piece::Pawn);
    ASSERT_EQ(Piece::getType(Piece::White | Piece::Rook), Piece::Rook);
    ASSERT_EQ(Piece::getType(Piece::White | Piece::Knight), Piece::Knight);
    ASSERT_EQ(Piece::getType(Piece::White | Piece::Bishop), Piece::Bishop);
    ASSERT_EQ(Piece::getType(Piece::White | Piece::Queen), Piece::Queen);
    ASSERT_EQ(Piece::getType(Piece::White | Piece::King), Piece::King);

    ASSERT_EQ(Piece::getType(Piece::Black | Piece::Pawn), Piece::Pawn);
    ASSERT_EQ(Piece::getType(Piece::Black | Piece::Rook), Piece::Rook);
    ASSERT_EQ(Piece::getType(Piece::Black | Piece::Knight), Piece::Knight);
    ASSERT_EQ(Piece::getType(Piece::Black | Piece::Bishop), Piece::Bishop);
    ASSERT_EQ(Piece::getType(Piece::Black | Piece::Queen), Piece::Queen);
    ASSERT_EQ(Piece::getType(Piece::Black | Piece::King), Piece::King);
}

TEST(Piece, isSlidingPiece) {
    ASSERT_FALSE(Piece::isSlidingPiece(Piece::White | Piece::Pawn));
    ASSERT_FALSE(Piece::isSlidingPiece(Piece::White | Piece::Knight));

    ASSERT_FALSE(Piece::isSlidingPiece(Piece::Black | Piece::Pawn));
    ASSERT_FALSE(Piece::isSlidingPiece(Piece::Black | Piece::Knight));

    ASSERT_TRUE(Piece::isSlidingPiece(Piece::White | Piece::Bishop));
    ASSERT_TRUE(Piece::isSlidingPiece(Piece::White | Piece::Rook));
    ASSERT_TRUE(Piece::isSlidingPiece(Piece::White | Piece::King));
    ASSERT_TRUE(Piece::isSlidingPiece(Piece::White | Piece::Queen));

    ASSERT_TRUE(Piece::isSlidingPiece(Piece::Black | Piece::Bishop));
    ASSERT_TRUE(Piece::isSlidingPiece(Piece::Black | Piece::Rook));
    ASSERT_TRUE(Piece::isSlidingPiece(Piece::Black | Piece::King));
    ASSERT_TRUE(Piece::isSlidingPiece(Piece::Black | Piece::Queen));
}

TEST(Piece, isLongRangeSlidingPiece) {
    ASSERT_FALSE(Piece::isLongRangeSlidingPiece(Piece::White | Piece::Pawn));
    ASSERT_FALSE(Piece::isLongRangeSlidingPiece(Piece::White | Piece::Knight));
    ASSERT_FALSE(Piece::isLongRangeSlidingPiece(Piece::White | Piece::King));

    ASSERT_FALSE(Piece::isLongRangeSlidingPiece(Piece::Black | Piece::Pawn));
    ASSERT_FALSE(Piece::isLongRangeSlidingPiece(Piece::Black | Piece::Knight));
    ASSERT_FALSE(Piece::isLongRangeSlidingPiece(Piece::Black | Piece::King));

    ASSERT_TRUE(Piece::isLongRangeSlidingPiece(Piece::White | Piece::Bishop));
    ASSERT_TRUE(Piece::isLongRangeSlidingPiece(Piece::White | Piece::Rook));
    ASSERT_TRUE(Piece::isLongRangeSlidingPiece(Piece::White | Piece::Queen));

    ASSERT_TRUE(Piece::isLongRangeSlidingPiece(Piece::Black | Piece::Bishop));
    ASSERT_TRUE(Piece::isLongRangeSlidingPiece(Piece::Black | Piece::Rook));
    ASSERT_TRUE(Piece::isLongRangeSlidingPiece(Piece::Black | Piece::Queen));
}

TEST(Piece, getValue) {
    ASSERT_EQ(Piece::getValue(Piece::Pawn | Piece::White), Piece::PawnValue);
    ASSERT_EQ(Piece::getValue(Piece::Rook | Piece::White), Piece::RookValue);
    ASSERT_EQ(Piece::getValue(Piece::Knight | Piece::White), Piece::KnightValue);
    ASSERT_EQ(Piece::getValue(Piece::Bishop | Piece::White), Piece::BishopValue);
    ASSERT_EQ(Piece::getValue(Piece::Queen | Piece::White), Piece::QueenValue);
    ASSERT_EQ(Piece::getValue(Piece::King | Piece::White), 0);

    ASSERT_EQ(Piece::getValue(Piece::Pawn | Piece::Black), Piece::PawnValue);
    ASSERT_EQ(Piece::getValue(Piece::Rook | Piece::Black), Piece::RookValue);
    ASSERT_EQ(Piece::getValue(Piece::Knight | Piece::Black), Piece::KnightValue);
    ASSERT_EQ(Piece::getValue(Piece::Bishop | Piece::Black), Piece::BishopValue);
    ASSERT_EQ(Piece::getValue(Piece::Queen | Piece::Black), Piece::QueenValue);
    ASSERT_EQ(Piece::getValue(Piece::King | Piece::Black), 0);
}