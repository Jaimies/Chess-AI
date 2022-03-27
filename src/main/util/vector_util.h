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

    template<class T>
    size_t indexOf(std::vector<T> &v, T element) {
        return std::find(v.begin(), v.end(), element) - v.begin();
    }

    template<class T>
    void move(std::vector<T> &v, size_t oldIndex, size_t newIndex) {
        auto elemAtNewIndex = v[newIndex];
        auto elemAtOldIndex = v[oldIndex];

        v[oldIndex] = elemAtNewIndex;
        v[newIndex] = elemAtOldIndex;
    }

    template<class T>
    void moveElem(std::vector<T> &v, T element, size_t newIndex) {
        size_t elemIndex = indexOf(v, element);
        move(v, elemIndex, newIndex);
    }
}
