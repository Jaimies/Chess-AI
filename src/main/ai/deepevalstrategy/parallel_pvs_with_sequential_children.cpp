#include "parallel_pvs_with_sequential_children.h"
#include "ai/single_depth_move_generator.h"

namespace DeepEvaluationStrategy {
    const Base *ParallelPvsWithSequentialChildren::getFirstMoveEvaluationStrategy() const {
        return generator->sequentialStrategy;
    }
}
