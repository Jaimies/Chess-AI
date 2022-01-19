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
    explicit Job(ThreadPool *threadPool, Job *parent = nullptr) : threadPool(threadPool), parent(parent) {}

    void execute(const std::function<void(bool)>& action);
    void awaitAll();
    void cancelAll();

    bool active() { return isActive; }

    Job *startNewJob();

private:
    ThreadPool *threadPool;
    Job *parent;
    std::vector<Job *> children;
    std::mutex childrenMutex;
    bool isActive = true;
    std::mutex threadsMutex;
    std::vector<std::thread *> threads;
};
