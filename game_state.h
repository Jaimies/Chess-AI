#pragma once

#include "Board.h"
#include "icon.h"

namespace GameState {
    static Board *board = Board::fromFenString(Board::startPosition);
    static std::array<Icon *, 64> possibleMoveIcons;
};
