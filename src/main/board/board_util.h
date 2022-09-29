#pragma once

#include <string>

namespace BoardUtil {
    extern int WhitePawnRank;
    extern int BlackPawnRank;

    int rank(int square);
    int file(int square);

    int squareFromPosition(std::string position);

    int initialRankOfPawn(int piece);
    bool isPawnAtStartSquare(int square, int piece);
}