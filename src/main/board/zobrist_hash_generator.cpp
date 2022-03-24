#include <array>
#include "Board.h"
#include "zobrist_hash_generator.h"

_ZobristHashGenerator ZobristHashGenerator;

uint64_t _ZobristHashGenerator::get64rand() {
    return dis(gen);
}

_ZobristHashGenerator::_ZobristHashGenerator() {
    srand(time(nullptr));
    for (unsigned int squareIndex = 0; squareIndex < 64; squareIndex++) {
        for (unsigned int pieceIndex = 0; pieceIndex < 12; pieceIndex++) {
            hashTable[squareIndex][pieceIndex] = get64rand();
        }
    }
}

uint64_t _ZobristHashGenerator::hash(Board *board) {
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

uint64_t _ZobristHashGenerator::hashPiece(int square, int piece) {
    return hashTable[square][piece];
}
