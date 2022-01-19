#include "threadpool.h"

Job *ThreadPool::startNewJob() {
    jobsMutex.lock();
    auto job = new Job(this, nullptr);
    jobs.emplace_back(job);
    jobsMutex.unlock();
    return job;
}

void ThreadPool::notifyThreadCountIncreased() {
    mutex.lock();
    activeThreadCount--;
    mutex.unlock();
}

void ThreadPool::notifyThreadCountDecreased() {
    mutex.lock();
    activeThreadCount++;
    mutex.unlock();
}

bool ThreadPool::shouldStartANewThread() const {
    return activeThreadCount < threadCount;
}

void Job::execute(const std::function<void(bool)> &action) {
    if (threadPool->shouldStartANewThread()) {
        threadsMutex.lock();
        threads.push_back(new std::thread([this, action]() {
            try {
                action(true);
                threadPool->notifyThreadCountDecreased();
            } catch (...) {}
        }));

        threadsMutex.unlock();
        threadPool->notifyThreadCountIncreased();
    } else
        action(false);
}

void Job::awaitAll() {
    threadsMutex.lock();
    for (auto thread: threads) {
        if (thread == nullptr || !thread->joinable()) continue;
        thread->join();
        delete thread;
    }
    threadsMutex.unlock();
}

void Job::cancelAll() {
    isActive = false;
    for (auto job: children) job->cancelAll();
}

Job *Job::startNewJob() {
    Job *job = new Job(threadPool, this);
    childrenMutex.lock();
    children.push_back(job);
    childrenMutex.unlock();
    return job;
}
