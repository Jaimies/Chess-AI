#include <stdexcept>
#include "piece.h"

namespace Piece {
    int getType(int piece) {
        return piece & 7;
    }

    int getColour(int piece) {
        return piece & 24;
    }

    bool isSlidingPiece(int piece) {
        auto type = getType(piece);
        return type == Queen || type == Bishop || type == Rook || type == King;
    }

    bool isLongRangeSlidingPiece(int piece) {
        auto type = getType(piece);
        return type == Queen || type == Bishop || type == Rook;
    }

    int getOpponentColour(int colour) {
        if (colour == None) throw std::invalid_argument("expected a colour, got None");
        return colour == White ? Black : White;
    }

    int getOpponentColourFromPiece(int piece) {
        return getOpponentColour(getColour(piece));
    }

    int getValue(int piece) {
        auto type = getType(piece);
        if (type == Pawn) return PawnValue;
        if (type == Knight) return KnightValue;
        if (type == Bishop) return BishopValue;
        if (type == Queen) return QueenValue;
        if (type == Rook) return RookValue;
        return 0;
    }
}
