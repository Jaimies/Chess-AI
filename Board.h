#pragma once

#include <string>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "Piece.h"

class Move;

class Board {
public:
    std::array<int, 64> squares;
    std::vector<Move> legalMoves;

    int colourToMove = Piece::White;

    bool hasLegalMoves = true;
    bool isKingUnderAttack = false;

    std::unordered_map<int, bool> castlingPieceMoved{
            {Piece::King | Piece::White,      false},
            {Piece::King | Piece::Black,      false},
            {Piece::LeftRook | Piece::White,  false},
            {Piece::RightRook | Piece::White, false},
            {Piece::LeftRook | Piece::Black,  false},
            {Piece::RightRook | Piece::Black, false},
    };

    std::unordered_map<char, int> pieceTypeFromSymbol = {
            {'k', Piece::King},
            {'q', Piece::Queen},
            {'r', Piece::Rook},
            {'b', Piece::Bishop},
            {'p', Piece::Pawn},
            {'n', Piece::Knight},
    };

    static Board *fromFenString(std::string fenString, int colourToMove = Piece::White);
};
