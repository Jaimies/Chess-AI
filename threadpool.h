#pragma once

#include <thread>
#include <functional>
#include <mutex>

class Job;

class ThreadPool {
public:
    bool shouldStartANewThread() const;
    Job *startNewJob();

private :
    std::vector<Job *> jobs;
    std::mutex mutex;
    std::mutex jobsMutex;
    unsigned int threadCount = std::thread::hardware_concurrency();
    unsigned int activeThreadCount = 0;

    void notifyThreadCountIncreased();
    void notifyThreadCountDecreased();

    friend Job;
};

class Job {
public:
    explicit Job(ThreadPool *threadPool) : threadPool(threadPool) {}

    void execute(const std::function<void(bool)>& action);
    void awaitAll();

private:
    ThreadPool *threadPool;
    std::mutex threadsMutex;
    std::vector<std::thread *> threads;
};
