#include <iostream>
#include "Board.cpp"

int main() {
    auto board = Board::fromFenString(Board::startPosition);
    std::cout << board->legalMoves.size() << std::endl;
    return 0;
}
