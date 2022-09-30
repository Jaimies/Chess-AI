#include "board_util.h"
#include "piece.h"

namespace BoardUtil {
    int WhitePawnRank = 1;
    int WhitePieceRank = 0;
    int BlackPawnRank = 6;
    int BlackPieceRank = 7;

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

    int initialRankOfRook(int colour) {
        return colour == Piece::White ? WhitePieceRank : BlackPieceRank;
    }

    bool isPawnAtStartSquare(int square, int piece) {
        return rank(square) == initialRankOfPawn(piece);
    }

    bool isRookAtStartRank(int square, int colour) {
        return rank(square) == initialRankOfRook(colour);
    }

    bool isPawnAboutToPromote(int square, int piece) {
        return rank(square) == initialRankOfPawn(Piece::getOpponentColourFromPiece(piece));
    }
}
