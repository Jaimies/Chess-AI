#pragma once

#include "Piece.h"

class MoveGenerationStrategy {
public:
    [[nodiscard]] virtual bool shouldAddMove(int targetPiece, int colour) const = 0;
    [[nodiscard]] virtual bool shouldStopGeneratingSlidingMoves(int targetPiece, int colour) const = 0;
};

class _NormalMoveGenerationStrategy : public MoveGenerationStrategy {
public:
    [[nodiscard]] bool shouldAddMove(int targetPiece, int colour) const override {
        return Piece::getColour(targetPiece) != colour;
    };

    [[nodiscard]] bool shouldStopGeneratingSlidingMoves(int targetPiece, int colour) const override {
        return targetPiece != Piece::None;
    };
};

class _CaptureGenerationStrategy : public _NormalMoveGenerationStrategy {
public:
    [[nodiscard]] bool shouldAddMove(int targetPiece, int colour) const override {
        return Piece::getColour(targetPiece) == Piece::getOpponentColour(colour);
    };
};

class _AttackedSquaresGenerationStrategy : public _NormalMoveGenerationStrategy {
    [[nodiscard]] bool shouldAddMove(int targetPiece, int colour) const override {
        return true;
    };

    [[nodiscard]] bool shouldStopGeneratingSlidingMoves(int targetPiece, int colour) const override {
        return (targetPiece != Piece::None
                && Piece::getType(targetPiece) != Piece::King)
               || Piece::getColour(targetPiece) == colour;
    };
};

extern MoveGenerationStrategy *NormalMoveGenerationStrategy;
extern MoveGenerationStrategy *CaptureGenerationStrategy;
extern MoveGenerationStrategy *AttackedSquaresGenerationStrategy;
