#include <vector>

namespace VectorUtil {
    template<class T, class Predicate>
    std::vector<T> filter(std::vector<T> input, Predicate predicate) {
        std::vector<T> output;
        std::copy_if(input.begin(), input.end(), std::inserter(output, output.begin()), predicate);
        return output;
    }
}
