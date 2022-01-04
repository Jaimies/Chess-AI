#pragma once

#include "Board.h"
#include "ui_piece.h"
#include <vector>

class DragWidget;

class GameManager {
public:
    Board *board = Board::fromFenString(Board::startPosition);

    explicit GameManager() {};
    void setup(DragWidget *wdg);
    void makeMove(Move *move);

private:
    std::vector<UiPiece *> pieces;

    UiPiece *getPieceAtSquare(int square);
};
