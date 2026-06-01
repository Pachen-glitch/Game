#pragma once

#include "JobQueue.h"

#include <atomic>
#include <thread>
#include <vector>

// Small worker pool for background tasks (generation, asset prep, pathfinding).
class ThreadPool {// Pool de threads
public:
    explicit ThreadPool(std::size_t workerCount = 2);// Constructor
    ~ThreadPool();// Destructor

    void submit(JobQueue::Job job);// Agrega un job a la cola
    void waitIdle();// Espera a que la cola este vacia
    std::size_t pendingJobs() const;// Devuelve el numero de jobs pendientes

private:
    void workerLoop();// Loop de los threads

    JobQueue queue;// Cola de jobs
    std::vector<std::thread> workers;// Vector de threads
    std::atomic<std::size_t> activeJobs{0};// Contador de jobs activos
    std::atomic<std::size_t> pendingCount{0};// Contador de jobs pendientes
};
