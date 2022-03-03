#pragma once

#include <vector>

namespace VectorUtil {
    template<class T, class Predicate>
    std::vector<T> filter(std::vector<T> input, Predicate predicate) {
        std::vector<T> output;
        std::copy_if(input.begin(), input.end(), std::inserter(output, output.begin()), predicate);
        return output;
    }

    template<class I, class O, class Mapper>
    std::vector<O> map(std::vector<I> input, Mapper mapper) {
        std::vector<O> output;
        std::transform(input.begin(), input.end(), std::back_inserter(output), mapper);
        return output;
    }
}
