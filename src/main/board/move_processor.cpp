#include "move_processor.h"
#include "board.h"

void MoveGenerationProcessor::processMove(MoveVariant move) {
    board->addMoveIfLegal(move);
}

void MoveGenerationProcessor::processEnPassantMove(EnPassantMove move) {
    MoveProcessor::processEnPassantMove(move);
    board->enPassantTargetSquare = move.targetSquare;
}

void AttackedSquaresGenerationProcessor::processMove(MoveVariant move) {
    auto basicMove = visit(GetBasicMoveVisitor, move);
    if (basicMove.targetSquare == board->kingSquare)
        board->attacksKing[basicMove.startSquare] = true;

    board->squaresAttackedByOpponent[basicMove.targetSquare] = true;
}

void LegalMoveSearchProcessor::processMove(MoveVariant move) {
    auto isLegal = board->isMoveLegal(move);
    if (isLegal) board->hasLegalMoves = true;
}

void CaptureGenerationProcessor::processEnPassantMove(EnPassantMove move) {
    MoveProcessor::processEnPassantMove(move);
}
