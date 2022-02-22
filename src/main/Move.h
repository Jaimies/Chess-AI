#pragma once

#include <cstdint>
#include <string>
#include <array>
#include "Piece.h"

class Board;

class Move {
public:
    int startSquare;
    int targetSquare;

    virtual bool canCapture() { return false; };

    virtual void apply(Board &board);
    virtual void undo(Board &board);

    virtual int getCapturedSquare() { return -1; };
    virtual uint64_t getZorbristHash(std::array<int, 64> squares);
    std::string toString() const;

    virtual ~Move() = default;
protected:
    Move(int startSquare, int targetSquare);
};

class NormalMove : public Move {
public:
    int capturedPiece;
    NormalMove(int startSquare, int targetSquare, int capturedPiece = Piece::None);

    static NormalMove * fromString(std::string str);

    void apply(Board &board) override;
    void undo(Board &board) override;

    bool canCapture() override { return true; }
    int getCapturedSquare() override { return targetSquare; }
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

    uint64_t getZorbristHash(std::array<int, 64> squares) override;

    bool canCapture() override { return false; }
    int getCapturedSquare() override { return capturedPawnPosition; }
};

class PromotionMove : public NormalMove {
public:
    int pieceToPromoteTo;

    PromotionMove(int startSquare, int targetSquare, int pieceToPromoteTo, int capturedPiece = Piece::None);

    void apply(Board &board) override;
    void undo(Board &board) override;

    uint64_t getZorbristHash(std::array<int, 64> squares) override;

    int getCapturedSquare() override { return targetSquare; }
};