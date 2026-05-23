#pragma once

#include "JobQueue.h"

#include <atomic>
#include <thread>
#include <vector>

// Small worker pool for background tasks (generation, asset prep, pathfinding).
class ThreadPool {
public:
    explicit ThreadPool(std::size_t workerCount = 2);
    ~ThreadPool();

    void submit(JobQueue::Job job);
    void waitIdle();
    std::size_t pendingJobs() const;

private:
    void workerLoop();

    JobQueue queue;
    std::vector<std::thread> workers;
    std::atomic<std::size_t> activeJobs{0};
    std::atomic<std::size_t> pendingCount{0};
};
