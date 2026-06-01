#include "JobQueue.h"

void JobQueue::push(Job job) {// Agrega un job a la cola
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (shutdownRequested) return;
        jobs.push_back(std::move(job));// Agrega el job a la cola
    }
    cv.notify_one();// Notifica a un thread
}

JobQueue::Job JobQueue::pop() {// Elimina un job de la cola 
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [this] { return !jobs.empty() || shutdownRequested; });// Espera a que la cola no este vacia o se haya cerrado

    if (jobs.empty()) return {};// Si la cola esta vacia, devuelve un job vacio

    Job job = std::move(jobs.front());// Obtiene el primer job de la cola
    jobs.pop_front();// Elimina el primer job de la cola
    return job;
}

void JobQueue::shutdown() {// Cierra la cola    
    {
        std::lock_guard<std::mutex> lock(mutex);
        shutdownRequested = true;// Establece el flag de shutdown
    }
    cv.notify_all();// Notifica a todos los threads
}

bool JobQueue::isShutdown() const {
    std::lock_guard<std::mutex> lock(mutex);
    return shutdownRequested;
}
