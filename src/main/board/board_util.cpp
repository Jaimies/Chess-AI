#include "board_util.h"

namespace BoardUtil {
    int rank(int square) {
        return square / 8;
    }

    int file(int square) {
        return square % 8;
    }

    int squareFromPosition(std::string position) {
        auto rank = position[0] - 'a';
        auto file = position[1] - '1';

        return file * 8 + rank;
    };
}
