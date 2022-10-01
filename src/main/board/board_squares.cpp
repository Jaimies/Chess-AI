#include "board_squares.h"

namespace BoardSquares {
    int fromString(std::string position) {
        auto rank = position[0] - 'a';
        auto file = position[1] - '1';

        return file * 8 + rank;
    };

    std::string toString(int position) {
        int file = position % 8;
        int rank = position / 8;

        return std::string(1, file + 'a') + (std::to_string(rank + 1));
    }

}