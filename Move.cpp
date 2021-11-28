#include "Board.h"
#include "Move.h"

Move::Move(int startSquare, int targetSquare)
        : startSquare(startSquare), targetSquare(targetSquare) {};

NormalMove::NormalMove(int startSquare, int targetSquare, int capturedPiece)
        : Move(startSquare, targetSquare), capturedPiece(capturedPiece) {}

CastlingMove::CastlingMove(int kingSquare, int rookSquare)
        : Move(kingSquare, rookSquare) {}

EnPassantMove::EnPassantMove(int startSquare, int targetSquare, int capturedPawn, int capturedPawnPosition)
        : NormalMove(startSquare, targetSquare, capturedPawn), capturedPawnPosition(capturedPawnPosition) {}

PromotionMove::PromotionMove(int startSquare, int targetSquare, int pieceToPromoteTo, int capturedPiece)
        : NormalMove(startSquare, targetSquare, capturedPiece), pieceToPromoteTo(pieceToPromoteTo) {}

void Move::apply(Board& board) {}

void Move::undo(Board& board) {}

void NormalMove::apply(Board& board) {
    auto piece = board.squares[startSquare];

    board.squares[targetSquare] = piece;
    board.squares[startSquare] = Piece::None;
}

void NormalMove::undo(Board& board) {
    auto piece = board.squares[targetSquare];

    board.squares[startSquare] = piece;
    board.squares[targetSquare] = capturedPiece;
}

void CastlingMove::apply(Board& board) {
    int king = board.squares[startSquare];
    int rook = board.squares[targetSquare];

    int kingTargetCell = getTargetCastlingPositionForKing();
    int rookTargetCell = getTargetCastlingPositionForRook();

    board.squares[startSquare] = Piece::None;
    board.squares[targetSquare] = Piece::None;

    board.squares[kingTargetCell] = king;
    board.squares[rookTargetCell] = rook;
}

void CastlingMove::undo(Board& board) {
    int kingPosition = getTargetCastlingPositionForKing();
    int rookPosition = getTargetCastlingPositionForRook();

    board.squares[startSquare] = board.squares[kingPosition];
    board.squares[targetSquare] = board.squares[rookPosition];

    board.squares[kingPosition] = Piece::None;
    board.squares[rookPosition] = Piece::None;
}


int CastlingMove::getTargetCastlingPositionForRook() {
    return startSquare + (targetSquare - startSquare) > 0 ? 1 : -1;
}

int CastlingMove::getTargetCastlingPositionForKing() {
    return startSquare + ((targetSquare - startSquare) > 0 ? 1 : -1) * 2;
}

void PromotionMove::apply(Board& board) {
    NormalMove::apply(board);

    int promotedPawn = board.squares[targetSquare];
    int colour = Piece::getColour(promotedPawn);

    board.squares[targetSquare] = pieceToPromoteTo | colour;
}

void PromotionMove::undo(Board& board) {
    NormalMove::undo(board);

    board.squares[startSquare] = Piece::Pawn | Piece::getOpponentColour(board.colourToMove);
}

void EnPassantMove::apply(Board& board) {
    int pawn = board.squares[startSquare];

    board.squares[targetSquare] = pawn;
    board.squares[startSquare] = Piece::None;
    board.squares[capturedPawnPosition] = Piece::None;
}

void EnPassantMove::undo(Board& board) {
    board.squares[capturedPawnPosition] = capturedPiece;
    board.squares[targetSquare] = Piece::None;
    board.squares[startSquare] = Piece::Pawn | Piece::getOpponentColourFromPiece(capturedPiece);
}
