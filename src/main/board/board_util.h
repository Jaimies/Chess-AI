#pragma once

#include <string>

namespace BoardUtil {
    int rank(int square);
    int file(int square);

    int squareFromPosition(std::string position);
}