#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

// Lightweight signal bus — decouples systems (combat, UI, audio, economy).
class EventBus {
public:
    using Handler = std::function<void()>;

    static EventBus& instance();
// Suscribe un evento y ejecuta el handler del evento
    void subscribe(const std::string& event, Handler handler);
    void emit(const std::string& event);

    void clear();

private:
    EventBus() = default;
    std::unordered_map<std::string, std::vector<Handler>> handlers;
};
