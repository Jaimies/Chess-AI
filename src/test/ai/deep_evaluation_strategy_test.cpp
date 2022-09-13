#include <gtest/gtest.h>
#include "../../main/ai/move_generator.h"
#include "../../main/ai/single_depth_move_generator.h"
#include "../../main/ai/evalstrategy/sequential_deep_evaluation_strategy.h"

template<class T>
void assertAllValuesAreTheSame(std::vector<T> values) {
    auto firstValue = values[0];

    for(int i = 1; i < values.size(); i++) {
        ASSERT_EQ(values[i], firstValue);
    }
}

TEST(DeepEvaluationStrategy, AllStrategiesReturnTheSameEval) {
    auto board = Board::fromFenString("r3kbnr/ppp1pppp/2n1q3/1B3b2/3P4/2N2N2/PPP2PPP/R1BQK2R b KQk - 0 1");
    auto singleDepthGenerator = new SingleDepthMoveGenerator(new MoveGenerator(), board, 4);

    std::vector<int64_t> evals{
            singleDepthGenerator->sequentialStrategy->deepEvaluate(board, 5),
            singleDepthGenerator->parallelStrategy->deepEvaluate(board, 5),
            singleDepthGenerator->pvsStrategy->deepEvaluate(board, 5),
            singleDepthGenerator->parallelPvsStrategy->deepEvaluate(board, 5),
    };

    assertAllValuesAreTheSame(evals);

    std::cout << evals[0] << std::endl;
}
