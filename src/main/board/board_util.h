#pragma once

#include <string>

namespace BoardUtil {
    extern int WhitePawnRank;
    extern int WhitePieceRank;
    extern int BlackPawnRank;
    extern int BlackPieceRank;

    bool isValidSquare(int square);

    int rank(int square);
    int file(int square);

    int squareFromPosition(std::string position);

    int initialRankOfPawn(int piece);
    int initialRankOfRook(int colour);

    bool isPawnAtStartSquare(int square, int piece);
    bool isRookAtStartRank(int square, int colour);
}