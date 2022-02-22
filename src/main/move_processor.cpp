#include "move_processor.h"
#include "Board.h"

void MoveGenerationProcessor::processMove(Move *move) {
    moves.push_back(move);
}

void AttackedSquaresGenerationProcessor::processMove(Move *move) {
    if (move->targetSquare == board->kingSquare)
        board->attacksKing[move->startSquare] = true;

    board->squaresAttackedByOpponent[move->targetSquare] = true;

    delete move;
}

void LegalMoveSearchProcessor::processMove(Move *move) {
    auto isLegal = board->isMoveLegal(move);
    if (isLegal) hasLegalMoves = true;
    delete move;
}
