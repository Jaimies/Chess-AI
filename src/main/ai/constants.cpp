#include "constants.h"

Evaluation const minEvaluation = std::numeric_limits<Evaluation>::min() + 10;
Evaluation const maxEvaluation = std::numeric_limits<Evaluation>::max() - 10;
Evaluation const checkmateEvaluation = minEvaluation + 1000;
