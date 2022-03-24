#pragma once

#include <array>
#include <random>
#include <climits>
#include "board.h"

class _ZobristHashGenerator;
extern _ZobristHashGenerator ZobristHashGenerator;

class _ZobristHashGenerator {
public:
    _ZobristHashGenerator();

    uint64_t hash(Board *board);
    uint64_t hashPiece(int square, int piece);
private:
    std::uniform_int_distribution<uint64_t> dis = std::uniform_int_distribution<uint64_t>(
            std::numeric_limits<uint64_t>::min(),
            std::numeric_limits<uint64_t>::max()
    );

    std::random_device rd;
    std::mt19937_64 gen = std::mt19937_64 (rd());

    std::array<std::array<uint64_t, 12>, 64> hashTable;
    uint64_t whiteLeftCastlingHash = get64rand();
    uint64_t whiteRightCastlingHash = get64rand();
    uint64_t blackLeftCastlingHash = get64rand();
    uint64_t blackRightCastlingHash = get64rand();

    uint64_t get64rand();
};
