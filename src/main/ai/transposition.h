#pragma once

#include <cstdint>

struct Transposition {
    uint64_t zobristKey;
    int64_t value;
    int depth;
    int type;

    const static int EXACT = 0;
    const static int UPPER = 1;
    const static int LOWER = 2;
};
