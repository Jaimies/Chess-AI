#pragma once

#include <cstdint>
#include <mutex>

class EvaluationUpdateStrategy {
public:
    virtual void updateEvaluation(int64_t evaluation, bool &shouldExit, int64_t &alpha, int64_t &beta) = 0;

protected:
    static void _updateEvaluation(int64_t evaluation, bool &shouldExit, int64_t &alpha, int64_t &beta);
};

class ParallelizedUpdateStrategy : public EvaluationUpdateStrategy {
    std::mutex mutex = std::mutex();

    void updateEvaluation(int64_t evaluation, bool &shouldExit, int64_t &alpha, int64_t &beta) override;
};

class NonParallelizedUpdateStrategy : public EvaluationUpdateStrategy {
    void updateEvaluation(int64_t evaluation, bool &shouldExit, int64_t &alpha, int64_t &beta) override;
};
