#include "board_util.h"
#include "piece.h"

namespace BoardUtil {
    bool isValidSquare(int square) {
        return square >= 0 && square <= 63;
    }

    int rank(int square) {
        return square / 8;
    }

    int file(int square) {
        return square % 8;
    }

    int initialRankOfPawn(int piece) {
        return Piece::isWhite(piece) ? WhitePawnRank : BlackPawnRank;
    }

    int initialRankOfRook(int colour) {
        return Piece::isWhite(colour) ? WhitePieceRank : BlackPieceRank;
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
