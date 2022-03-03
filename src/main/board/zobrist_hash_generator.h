#pragma once

#include <array>
#include "Board.h"

void generateHashes();
uint64_t get64rand();
unsigned long long int hash(Board *board);
uint64_t hashPiece(int square, int piece);
