#pragma once

#include <vector>
#include "Move.h"

class MoveProcessor {
public:
    virtual void processMove(MoveVariant move) = 0;
};

class MoveGenerationProcessor : public MoveProcessor {
public:
    explicit MoveGenerationProcessor(Board *board) :board(board) {}
    void processMove(MoveVariant move) override;
private:
    Board *board;
};

class AttackedSquaresGenerationProcessor : public MoveProcessor {
public:
    explicit AttackedSquaresGenerationProcessor(Board *board) : board(board) {}
    void processMove(MoveVariant move) override;

private:
    Board *board;
};

class LegalMoveSearchProcessor: public MoveProcessor {
public:
    explicit LegalMoveSearchProcessor(Board *board) : board(board) {}
    void processMove(MoveVariant move) override;

    bool hasLegalMoves = false;
private:
    Board *board;
};