#pragma once

#include <string>

namespace BoardUtil {
    const int WhitePawnRank = 1;
    const int WhitePieceRank = 0;
    const  int BlackPawnRank = 6;
    const int BlackPieceRank = 7;

    bool isValidSquare(int square);

    int rank(int square);
    int file(int square);

    int squareFromPosition(std::string position);

    int initialRankOfPawn(int piece);
    int initialRankOfRook(int colour);

    bool isPawnAtStartSquare(int square, int piece);
    bool isRookAtStartRank(int square, int colour);

    bool isPawnAboutToPromote(int square, int piece);
}