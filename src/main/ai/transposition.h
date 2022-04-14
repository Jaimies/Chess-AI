#pragma once

#include <cstdint>
#include <atomic>
#include <memory>

struct Transposition {
    int64_t value;
    int depth;
    int type;

    const static int EXACT = 0;
    const static int UPPER = 1;
    const static int LOWER = 2;
};

typedef std::shared_ptr<std::atomic<Transposition>> AtomicTranspositionPtr;
