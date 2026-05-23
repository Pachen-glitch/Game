#include "JobQueue.h"

void JobQueue::push(Job job) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (shutdownRequested) return;
        jobs.push_back(std::move(job));
    }
    cv.notify_one();
}

JobQueue::Job JobQueue::pop() {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [this] { return !jobs.empty() || shutdownRequested; });

    if (jobs.empty()) return {};

    Job job = std::move(jobs.front());
    jobs.pop_front();
    return job;
}

void JobQueue::shutdown() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        shutdownRequested = true;
    }
    cv.notify_all();
}

bool JobQueue::isShutdown() const {
    std::lock_guard<std::mutex> lock(mutex);
    return shutdownRequested;
}
