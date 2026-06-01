#include "ThreadPool.h"

ThreadPool::ThreadPool(std::size_t workerCount) {// Constructor 
    if (workerCount == 0) workerCount = 1;
    workers.reserve(workerCount);// Reserva espacio para los threads
    for (std::size_t i = 0; i < workerCount; ++i) {
        workers.emplace_back([this] { workerLoop(); });// Crea un thread
    }
}

ThreadPool::~ThreadPool() {// Destructor
    queue.shutdown();// Cierra la cola
    for (auto& worker : workers) {// Espera a que todos los threads terminen
        if (worker.joinable()) worker.join();// Espera a que el thread termine
    }
}

void ThreadPool::submit(JobQueue::Job job) {// Agrega un job a la cola
    pendingCount.fetch_add(1, std::memory_order_relaxed);// Incrementa el contador de jobs pendientes
    queue.push([this, job = std::move(job)]() mutable {
        job();// Ejecuta el job
        pendingCount.fetch_sub(1, std::memory_order_relaxed);// Decrementa el contador de jobs pendientes
    });
}

void ThreadPool::waitIdle() {// Espera a que la cola este vacia
    while (pendingCount.load(std::memory_order_relaxed) > 0) {// Espera a que el contador de jobs pendientes sea 0      
        std::this_thread::yield();// Espera a que el thread se libere
    }
}

std::size_t ThreadPool::pendingJobs() const {// Devuelve el numero de jobs pendientes   
    return pendingCount.load(std::memory_order_relaxed);// Devuelve el contador de jobs pendientes
}

void ThreadPool::workerLoop() {// Loop de los threads
    while (!queue.isShutdown()) {
        JobQueue::Job job = queue.pop();// Elimina un job de la cola
        if (!job) continue;// Si el job es null, continua
        activeJobs.fetch_add(1, std::memory_order_relaxed);// Incrementa el contador de jobs activos
        job();// Ejecuta el job
        activeJobs.fetch_sub(1, std::memory_order_relaxed);// Decrementa el contador de jobs activos
    }
}
