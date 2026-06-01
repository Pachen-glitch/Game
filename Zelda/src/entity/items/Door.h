#pragma once

#include "../base/Entity.h"

enum class DoorKind {
    Normal,
    BossGate
};

class Door : public Entity {
public:
    Door(
        sf::Vector2f pos,
        bool isLocked = true,
        DoorKind kind = DoorKind::Normal
    );

    void unlock();
    bool isLocked() const;
    bool canPass() const;
    bool isBossGate() const { return doorKind == DoorKind::BossGate; }

    void onInteract(Player& player) override;
    void update(float dt) override;

private:
    bool locked = true;
    DoorKind doorKind = DoorKind::Normal;
};
