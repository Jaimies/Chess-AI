#include "Board.h"
#include "Move.h"
#include "zobrist_hash_generator.h"

Move::Move(int startSquare, int targetSquare)
        : startSquare(startSquare), targetSquare(targetSquare) {};

NormalMove::NormalMove(int startSquare, int targetSquare, int capturedPiece)
        : Move(startSquare, targetSquare), capturedPiece(capturedPiece) {}

CastlingMove::CastlingMove(int kingSquare, int targetSquare, int rookSquare, int rookTargetSquare)
        : Move(kingSquare, targetSquare), rookSquare(rookSquare), rookTargetSquare(rookTargetSquare) {}

EnPassantMove::EnPassantMove(int startSquare, int targetSquare, int capturedPawn, int capturedPawnPosition)
        : NormalMove(startSquare, targetSquare, capturedPawn), capturedPawnPosition(capturedPawnPosition) {}

PromotionMove::PromotionMove(int startSquare, int targetSquare, int pieceToPromoteTo, int capturedPiece)
        : NormalMove(startSquare, targetSquare, capturedPiece), pieceToPromoteTo(pieceToPromoteTo) {}

void Move::apply(Board &board) {}

void Move::undo(Board &board) {}

static std::string toString(int position) {
    int file = position % 8;
    int rank = position / 8;

    return std::string(1, file + 'a') + (std::to_string(rank + 1));
}

std::string Move::toString() const {
    return ::toString(startSquare) + ::toString(targetSquare);
}

uint64_t Move::getZorbristHash(std::array<int, 64> squares) {
    auto piece = squares[startSquare];
    return hashPiece(startSquare, piece) ^ hashPiece(targetSquare, piece);
}

int getSquareFromPosition(char file, char rank) {
    int rankNumber = rank - '0' - 1;
    int fileNumber = file - 'a';
    return rankNumber * 8 + fileNumber;
}

NormalMove *NormalMove::fromString(std::string str) {
    int startSquare = getSquareFromPosition(str[0], str[1]);
    int targetSquare = getSquareFromPosition(str[2], str[3]);
    return new NormalMove(startSquare, targetSquare);
}

void NormalMove::apply(Board &board) {
    auto piece = board.squares[startSquare];

    board.squares[targetSquare] = piece;
    board.squares[startSquare] = Piece::None;
}

void NormalMove::undo(Board &board) {
    auto piece = board.squares[targetSquare];

    board.squares[startSquare] = piece;
    board.squares[targetSquare] = capturedPiece;
}

void CastlingMove::apply(Board &board) {
    int king = board.squares[startSquare];
    int rook = board.squares[rookSquare];

    board.squares[startSquare] = Piece::None;
    board.squares[targetSquare] = king;

    board.squares[rookSquare] = Piece::None;
    board.squares[rookTargetSquare] = rook;
}

void CastlingMove::undo(Board &board) {
    board.squares[startSquare] = board.squares[targetSquare];
    board.squares[rookSquare] = board.squares[rookTargetSquare];

    board.squares[targetSquare] = Piece::None;
    board.squares[rookTargetSquare] = Piece::None;
}

void PromotionMove::apply(Board &board) {
    NormalMove::apply(board);

    int promotedPawn = board.squares[targetSquare];
    int colour = Piece::getColour(promotedPawn);

    board.squares[targetSquare] = pieceToPromoteTo | colour;
}

void PromotionMove::undo(Board &board) {
    NormalMove::undo(board);

    board.squares[startSquare] = Piece::Pawn | Piece::getOpponentColour(board.colourToMove);
}

uint64_t PromotionMove::getZorbristHash(std::array<int, 64> squares) {
    return hashPiece(startSquare, squares[startSquare]) ^ hashPiece(targetSquare, pieceToPromoteTo);
}

void EnPassantMove::apply(Board &board) {
    int pawn = board.squares[startSquare];

    board.squares[targetSquare] = pawn;
    board.squares[startSquare] = Piece::None;
    board.squares[capturedPawnPosition] = Piece::None;
}

void EnPassantMove::undo(Board &board) {
    board.squares[capturedPawnPosition] = capturedPiece;
    board.squares[targetSquare] = Piece::None;
    board.squares[startSquare] = Piece::Pawn | Piece::getOpponentColourFromPiece(capturedPiece);
}

uint64_t EnPassantMove::getZorbristHash(std::array<int, 64> squares) {
    return Move::getZorbristHash(squares)
           ^ hashPiece(capturedPawnPosition, squares[capturedPawnPosition]);
}
