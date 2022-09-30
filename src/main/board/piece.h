#pragma once

#include <unordered_map>

namespace Piece {
    const int None = 0;
    const int King = 1;
    const int Queen = 2;
    const int Bishop = 3;
    const int Knight = 4;
    const int Rook = 5;
    const int Pawn = 6;

    const int White = 8;
    const int Black = 16;

    const int LeftRook = 32;
    const int RightRook = 64;

    const int PawnValue = 10000;
    const int BishopValue = 30000;
    const int KnightValue = 30000;
    const int RookValue = 50000;
    const int QueenValue = 90000;

    int getColour(int piece);
    int getType(int piece);
    bool isSlidingPiece(int piece);
    bool isLongRangeSlidingPiece(int piece);
    int getOpponentColour(int colour);
    int getOpponentColourFromPiece(int piece);
    int getValue(int piece);

    constexpr const int piecesToPromoteTo[]{Queen, Rook, Bishop, Knight};
};
