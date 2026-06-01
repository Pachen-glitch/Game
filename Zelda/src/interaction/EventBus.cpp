#include "EventBus.h"

EventBus& EventBus::instance() { // Devuelve la instancia del EventBus
    static EventBus bus;
    return bus;
}
// Suscribe un evento y ejecuta el handler del evento
void EventBus::subscribe(const std::string& event, Handler handler) { // Suscribe un evento
    handlers[event].push_back(std::move(handler));
}
// Emite un evento y ejecuta los handlers del evento
void EventBus::emit(const std::string& event) { // Emite un evento
    auto it = handlers.find(event); // Busca el evento en el map
    if (it == handlers.end()) return;
    for (auto& h : it->second) { // Ejecuta los handlers del evento
        h();
    }
}

void EventBus::clear() { // Elimina todos los eventos
    handlers.clear();
}
