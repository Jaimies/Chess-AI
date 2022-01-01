#include <vector>

#pragma once

namespace VectorUtil {
    template<class T, class Predicate>
    std::vector<T> filter(std::vector<T> input, Predicate predicate);
}
