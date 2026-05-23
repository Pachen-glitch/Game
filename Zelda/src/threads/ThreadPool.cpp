#include "ThreadPool.h"

ThreadPool::ThreadPool(std::size_t workerCount) {
    if (workerCount == 0) workerCount = 1;
    workers.reserve(workerCount);
    for (std::size_t i = 0; i < workerCount; ++i) {
        workers.emplace_back([this] { workerLoop(); });
    }
}

ThreadPool::~ThreadPool() {
    queue.shutdown();
    for (auto& worker : workers) {
        if (worker.joinable()) worker.join();
    }
}

void ThreadPool::submit(JobQueue::Job job) {
    pendingCount.fetch_add(1, std::memory_order_relaxed);
    queue.push([this, job = std::move(job)]() mutable {
        job();
        pendingCount.fetch_sub(1, std::memory_order_relaxed);
    });
}

void ThreadPool::waitIdle() {
    while (pendingCount.load(std::memory_order_relaxed) > 0) {
        std::this_thread::yield();
    }
}

std::size_t ThreadPool::pendingJobs() const {
    return pendingCount.load(std::memory_order_relaxed);
}

void ThreadPool::workerLoop() {
    while (!queue.isShutdown()) {
        JobQueue::Job job = queue.pop();
        if (!job) continue;
        activeJobs.fetch_add(1, std::memory_order_relaxed);
        job();
        activeJobs.fetch_sub(1, std::memory_order_relaxed);
    }
}
