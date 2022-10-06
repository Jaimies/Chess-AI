#include "evaluation_update_strategy.h"
#include "transposition.h"

void EvaluationUpdateStrategy::_updateEvaluation(int64_t evaluation, bool &shouldExit, int64_t &alpha, int64_t &beta) {
    if (evaluation >= beta) {
        shouldExit = true;
        alpha = beta;
        return;
    }
    if (evaluation > alpha) {
        alpha = evaluation;
    }
}

void ParallelizedUpdateStrategy::updateEvaluation(int64_t evaluation, bool &shouldExit, int64_t &alpha, int64_t &beta) {
    mutex.lock();
    _updateEvaluation(evaluation, shouldExit, alpha, beta);
    mutex.unlock();
}

void NonParallelizedUpdateStrategy::updateEvaluation(int64_t evaluation, bool &shouldExit, int64_t &alpha, int64_t &beta) {
    _updateEvaluation(evaluation, shouldExit, alpha, beta);
}
