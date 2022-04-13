#pragma once

#include <vector>
#include "../move/move.h"

class MoveProcessor {
public:
    virtual void processMove(MoveVariant move) = 0;
    [[nodiscard]] virtual bool shouldAddMove(int targetPiece, int colour) const = 0;
    [[nodiscard]] virtual bool shouldStopGeneratingSlidingMoves(int targetPiece, int colour) const = 0;
};

class MoveGenerationProcessor : public MoveProcessor {
public:
    explicit MoveGenerationProcessor(Board *board) :board(board) {}
    void processMove(MoveVariant move) override;

    [[nodiscard]] bool shouldAddMove(int targetPiece, int colour) const override {
        return Piece::getColour(targetPiece) != colour;
    };

    [[nodiscard]] bool shouldStopGeneratingSlidingMoves(int targetPiece, int colour) const override {
        return targetPiece != Piece::None;
    };
protected:
    Board *board;
};

class CaptureGenerationProcessor: public MoveGenerationProcessor {
public:
    explicit CaptureGenerationProcessor(Board *board) : MoveGenerationProcessor(board) {}


    [[nodiscard]] bool shouldAddMove(int targetPiece, int colour) const override {
        return Piece::getColour(targetPiece) == Piece::getOpponentColour(colour);
    };
};

class AttackedSquaresGenerationProcessor : public MoveProcessor {
public:
    explicit AttackedSquaresGenerationProcessor(Board *board) : board(board) {}
    void processMove(MoveVariant move) override;

    [[nodiscard]] bool shouldAddMove(int targetPiece, int colour) const override {
        return true;
    };

    static bool isCapturable(int piece) {
        return piece != Piece::None && Piece::getType(piece) != Piece::King;
    }

    [[nodiscard]] bool shouldStopGeneratingSlidingMoves(int targetPiece, int colour) const override {
        return Piece::getColour(targetPiece) == colour
                || isCapturable(targetPiece);
    };

private:
    Board *board;
};

class LegalMoveSearchProcessor: public MoveGenerationProcessor {
public:
    explicit LegalMoveSearchProcessor(Board *board): MoveGenerationProcessor(board) {}
    void processMove(MoveVariant move) override;
};