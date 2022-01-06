#pragma once

#include "Board.h"
#include "ui_piece.h"
#include "promotion_dialog.h"
#include <vector>

class DragWidget;

class GameManager {
public:
    Board *board = Board::fromFenString(Board::startPosition);

    explicit GameManager() {};
    void setup(DragWidget *wdg);
    void makeMove(Move *move, bool isMachineMove = false);

private:
    std::vector<UiPiece *> pieces;
    PromotionDialog *promotionDialog;
    PromotionDialogOverlay *promotionDialogBackground;

    UiPiece *getPieceAtSquare(int square);
    void makeMachineMoveIfNecessary();
};
