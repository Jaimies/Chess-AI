#pragma once

class Board;

class Move {
public:
    int startSquare;
    int targetSquare;

    virtual bool canCapture() { return false; };

    virtual void apply(Board &board);
    virtual void undo(Board &board);

    virtual ~Move() = default;
protected:
    Move(int startSquare, int targetSquare);
};

class NormalMove : public Move {
public:
    int capturedPiece;
    NormalMove(int startSquare, int targetSquare, int capturedPiece = Piece::None);

    void apply(Board &board) override;
    void undo(Board &board) override;

    bool canCapture() override { return true; }
};

class CastlingMove : public Move {
public:
    int rookSquare;
    int rookTargetSquare;

    CastlingMove(int kingSquare, int targetSquare, int rookSquare, int rookTargetSquare);

    void apply(Board &board) override;
    void undo(Board &board) override;
};

class EnPassantMove : public NormalMove {
public:
    int capturedPawnPosition;

    EnPassantMove(int startSquare, int targetSquare, int capturedPawn, int capturedPawnPosition);

    void apply(Board &board) override;
    void undo(Board &board) override;

    bool canCapture() override { return false; }
};

class PromotionMove : public NormalMove {
public:
    int pieceToPromoteTo;

    PromotionMove(int startSquare, int targetSquare, int pieceToPromoteTo, int capturedPiece = Piece::None);

    void apply(Board &board) override;
    void undo(Board &board) override;
};