#pragma once

namespace Piece {
    const int None        = 0b0000000;

    const int King        = 0b0000001;
    const int Queen       = 0b0000010;
    const int Bishop      = 0b0000011;
    const int Knight      = 0b0000100;
    const int Rook        = 0b0000101;
    const int Pawn        = 0b0000110;

    const int White       = 0b0001000;
    const int Black       = 0b0010000;

    const int LeftRook    = 0b0100000;
    const int RightRook   = 0b1000000;

    const int PawnValue   = 10000;
    const int BishopValue = 30000;
    const int KnightValue = 30000;
    const int RookValue   = 50000;
    const int QueenValue  = 90000;

    int getColour(int piece);
    int getOpponentColour(int colour);
    int getOpponentColourFromPiece(int piece);

    bool isOfColour(int piece, int colour);
    bool isWhite(int piece);
    bool isBlack(int piece);

    int getType(int piece);

    bool isSlidingPiece(int piece);
    bool isLongRangeSlidingPiece(int piece);

    int getValue(int piece);

    constexpr const int piecesToPromoteTo[]{Queen, Rook, Bishop, Knight};
};
