#include "EventBus.h"

EventBus& EventBus::instance() {
    static EventBus bus;
    return bus;
}

void EventBus::subscribe(const std::string& event, Handler handler) {
    handlers[event].push_back(std::move(handler));
}

void EventBus::emit(const std::string& event) {
    auto it = handlers.find(event);
    if (it == handlers.end()) return;
    for (auto& h : it->second) {
        h();
    }
}

void EventBus::clear() {
    handlers.clear();
}
