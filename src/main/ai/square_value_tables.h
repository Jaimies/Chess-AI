#pragma once

#include <cstdint>
#include <array>
#include "../board/board.h"

typedef std::array<int64_t, 64> SquareValueTable;

extern const SquareValueTable *kingMidGameSquareValues;
extern const SquareValueTable *kingEndGameSquareValues;
extern const SquareValueTable *queenSquareValues;
extern const SquareValueTable *bishopSquareValues;
extern const SquareValueTable *knightSquareValues;
extern const SquareValueTable *rookSquareValues;
extern const SquareValueTable *pawnSquareValues;

const SquareValueTable *getSquareValueTable(Board *board, int piece);