#include "evaluation.h"

Evaluation getPiecePositionValue(Board *board, int piece, int position) {
    auto squareValueTable = getSquareValueTable(board, piece);
    auto positionToAccess = Piece::isWhite(piece) ? 63 - position : position;
    return (*squareValueTable)[positionToAccess] * 10;
}

int64_t performEvaluation(Board *board) {
    long sum = 0;

    for (int square = 0; square < 64; square++) {
        auto piece = board->squares[square];
        if (piece == Piece::None) continue;
        auto squareEvaluation = Piece::getValue(piece) + getPiecePositionValue(board, piece, square);
        if (Piece::getColour(piece) == board->colourToMove) sum += squareEvaluation;
        else sum -= squareEvaluation;
    }

    return sum;
}

Evaluation evaluatePositionWithoutMoves(Board *board, int depth) {
    // prefer to checkmate sooner, rather than later
    if (board->isKingUnderAttack) return checkmateEvaluation - depth;
    return 0;
}

long MoveGenerator::evaluate(Board *board, int depth) {
    if (!board->hasLegalMoves)
        return evaluatePositionWithoutMoves(board, depth);

    return performEvaluation(board);
}
