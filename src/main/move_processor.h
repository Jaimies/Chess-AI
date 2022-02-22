#pragma once

#include <vector>
#include "Move.h"

class MoveProcessor {
public:
    virtual void processMove(Move *) = 0;
};

class MoveGenerationProcessor : public MoveProcessor {
public:
    std::vector<Move *> moves;
    void processMove(Move *move) override;
};

class AttackedSquaresGenerationProcessor : public MoveProcessor {
public:
    explicit AttackedSquaresGenerationProcessor(Board *board) : board(board) {}
    void processMove(Move *move) override;

private:
    Board *board;
};

class LegalMoveSearchProcessor: public MoveProcessor {
public:
    explicit LegalMoveSearchProcessor(Board *board) : board(board) {}
    void processMove(Move *move) override;

    bool hasLegalMoves = false;
private:
    Board *board;
};