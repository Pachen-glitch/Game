#include "EntityManager.h"

#include <algorithm>

void EntityManager::updateAll(float dt) {
    for (auto& e : entities) {
        if (e && e->isActive()) {
            e->update(dt);
        }
    }
}

void EntityManager::removeInactive() {
    entities.erase(
        std::remove_if(entities.begin(), entities.end(),
            [](const std::unique_ptr<Entity>& e) {
                return e && !e->isActive();
            }),
        entities.end()
    );
}

std::vector<std::unique_ptr<Entity>>& EntityManager::all() {
    return entities;
}

const std::vector<std::unique_ptr<Entity>>& EntityManager::all() const {
    return entities;
}

void EntityManager::clear() {
    entities.clear();
}
