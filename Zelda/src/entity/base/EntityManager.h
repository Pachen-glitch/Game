#pragma once

#include "Entity.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

class Player;

// Central registry for dynamic entities in the current room.
class EntityManager {
public:
    template<typename T, typename... Args>
    T* spawn(Args&&... args) {
        auto entity = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = entity.get();
        entities.push_back(std::move(entity));
        return ptr;
    }

    void updateAll(float dt);
    void removeInactive();

    std::vector<std::unique_ptr<Entity>>& all();
    const std::vector<std::unique_ptr<Entity>>& all() const;

    void clear();

private:
    std::vector<std::unique_ptr<Entity>> entities;
};
