#include <array>
#include <random>
#include "Board.h"

std::uniform_int_distribution<uint64_t> dis(
        std::numeric_limits<uint64_t>::min(),
        std::numeric_limits<uint64_t>::max()
);

std::random_device rd;
std::mt19937_64 gen(rd());

uint64_t get64rand() {
    return dis(gen);
}

std::array<std::array<uint64_t, 12>, 64> hashTable;
uint64_t whiteLeftCastlingHash = get64rand();
uint64_t whiteRightCastlingHash = get64rand();
uint64_t blackLeftCastlingHash = get64rand();
uint64_t blackRightCastlingHash = get64rand();

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
        if (piece != 0) hash ^= hashTable[square][piece];
    }

    if (board->canWhiteCastleLeft())
        hash ^= whiteLeftCastlingHash;

    if (board->canWhiteCastleRight())
        hash ^= whiteRightCastlingHash;

    if (board->canBlackCastleLeft())
        hash ^= blackLeftCastlingHash;

    if (board->canBlackCastleRight())
        hash ^= blackRightCastlingHash;

    return hash;
}

uint64_t hashPiece(int square, int piece) {
    return hashTable[square][piece];
}
