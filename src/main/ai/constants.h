#pragma once

#include <limits>
#include <cstdint>

typedef int64_t Eval;

namespace EvalValues {
    const Eval min = std::numeric_limits<Eval>::min() + 10;
    const Eval max = std::numeric_limits<Eval>::max() - 10;
    const Eval checkmate = min + 1000;
}
