#include "move_sorting.h"

int64_t guessMoveValue(const Board *board, MoveVariant &move) {
    auto canCapture = visit(DetermineIfMoveCanCaptureVisitor, move);
    auto addedValue = visit(GetMoveAddedValueVisitor, move);
    auto basicMove = visit(GetBasicMoveVisitor, move);
    auto movePieceType = Piece::getType(board->squares[basicMove.startSquare]);
    auto capturePieceType = canCapture ? Piece::getType(board->squares[basicMove.targetSquare]) : Piece::None;

    int moveScoreGuess = 10 * (Piece::getValue(capturePieceType) + addedValue) - Piece::getValue(movePieceType);

    return moveScoreGuess;
}

void sortMoves(Board *board, std::vector<MoveVariant> &moves) {
    std::sort(moves.begin(), moves.end(), [board](MoveVariant &move, MoveVariant otherMove) {
        return guessMoveValue(board, move) > guessMoveValue(board, otherMove);
    });
}
