#pragma once

#include "Board.h"

class GameManager {
public:
    Board *board = Board::fromFenString(Board::startPosition);

    explicit GameManager() {};
};