#pragma once

#include <unordered_map>

class Piece {
public:
    static const int None = 0;
    static const int King = 1;
    static const int Queen = 2;
    static const int Bishop = 3;
    static const int Knight = 4;
    static const int Rook = 5;
    static const int Pawn = 6;

    static const int White = 8;
    static const int Black = 16;

    static const int LeftRook = 32;
    static const int RightRook = 64;

    static const int PawnValue = 10000;
    static const int BishopValue = 30000;
    static const int KnightValue = 30000;
    static const int RookValue = 50000;
    static const int QueenValue = 90000;

    static int getColour(int piece);
    static int getType(int piece);
    static bool isSlidingPiece(int piece);
    static bool isLongRangeSlidingPiece(int piece);
    static int getOpponentColour(int colour);
    static int getOpponentColourFromPiece(int piece);
    static int getValue(int piece);

    constexpr static const int piecesToPromoteTo[]{Queen, Rook, Bishop, Knight};
};
