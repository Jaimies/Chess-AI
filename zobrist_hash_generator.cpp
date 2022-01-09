#include <array>
#include "Board.h"

std::array<std::array<uint64_t, 12>, 64> hashTable;

uint64_t get64rand() {
    return
            (((uint64_t) rand() <<  0) & 0x000000000000FFFFull) |
            (((uint64_t) rand() << 16) & 0x00000000FFFF0000ull) |
            (((uint64_t) rand() << 32) & 0x0000FFFF00000000ull) |
            (((uint64_t) rand() << 48) & 0xFFFF000000000000ull);
}

void generateHashes() {
    srand(time(nullptr));
    for (unsigned int squareIndex = 0; squareIndex < 64; squareIndex++) {
        for (unsigned int pieceIndex = 0; pieceIndex < 12; pieceIndex++) {
            hashTable[squareIndex][pieceIndex] = get64rand();
        }
    }
}

uint64_t hash(Board *board) {
    unsigned long long int hash = 0;

    for (unsigned int square = 0; square < 64; square++) {
        auto piece = board->squares[square];
        if (piece == 0) continue;
        hash ^= hashTable[square][piece];
    }

    return hash;
}

