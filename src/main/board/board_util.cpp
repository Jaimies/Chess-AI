#include "board_util.h"
#include "piece.h"

namespace BoardUtil {
    int WhitePawnRank = 1;
    int BlackPawnRank = 6;

    bool isValidSquare(int square) {
        return square >= 0 && square <= 63;
    }

    int rank(int square) {
        return square / 8;
    }

    int file(int square) {
        return square % 8;
    }

    int squareFromPosition(std::string position) {
        auto rank = position[0] - 'a';
        auto file = position[1] - '1';

        return file * 8 + rank;
    };

    int initialRankOfPawn(int piece) {
        int colour = Piece::getColour(piece);

        if (colour == Piece::None) throw std::invalid_argument("Expected a pawn with a colour, got one with Piece::None");
        return colour == Piece::White ? WhitePawnRank : BlackPawnRank;
    }

    bool isPawnAtStartSquare(int square, int piece) {
        return rank(square) == initialRankOfPawn(piece);
    }
}
