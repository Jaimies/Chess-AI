#include <stdexcept>
#include "Piece.h"

int Piece::getType(int piece) {
    return piece & 7;
}

int Piece::getColour(int piece) {
    return piece & 24;
}

bool Piece::isSlidingPiece(int piece) {
    auto type = getType(piece);
    return type == Queen || type == Bishop || type == Rook || type == King;
}

bool Piece::isLongRangeSlidingPiece(int piece) {
    auto type = getType(piece);
    return type == Queen || type == Bishop || type == Rook;
}

int Piece::getOpponentColour(int colour) {
    if (colour == None) throw std::invalid_argument("expected a colour, got None");
    return colour == White ? Black : White;
}

int Piece::getOpponentColourFromPiece(int piece) {
    return getOpponentColour(getColour(piece));
}

int Piece::getValue(int piece) {
    auto type = getType(piece);
    if (type == Pawn) return 1;
    if (type == King) return 0;
    if (type == Knight || type == Bishop) return 3;
    if (type == Queen) return 9;
    if (type == Rook) return 5;
    return 0;
}
