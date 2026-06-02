#include "Door.h"

#include "../player/Player.h"

#include "../../core/Constants.h"
#include "../../utils/AssetPaths.h"
#include "../../interaction/EventBus.h"
#include "../../save/RunScoreTracker.h"

#include <cmath>

namespace {

const sf::Vector2f kLargeDoorSize{64.f, 80.f};
const sf::Vector2f kNormalDoorSize{48.f, 64.f};

} // namespace

Door::Door(
    sf::Vector2f pos,
    bool isLocked,
    DoorKind kind
)
    : Entity(
        pos,
        (kind == DoorKind::BossGate || kind == DoorKind::Victory)
            ? kLargeDoorSize
            : kNormalDoorSize,
        AssetPaths::getDoorSprite(),
        EntityType::Door
    )
    , locked(isLocked)
    , doorKind(kind)
{
    const bool largeDoor =
        doorKind == DoorKind::BossGate || doorKind == DoorKind::Victory;
    float scale = largeDoor ? 2.4f : 2.f;
    sprite.setScale(scale, scale);

    if (doorKind == DoorKind::Victory) {
        sprite.setColor(sf::Color(255, 220, 80));
    } else if (doorKind == DoorKind::BossGate) {
        sprite.setColor(locked ? sf::Color(255, 180, 60) : sf::Color(120, 255, 120));
    } else if (!locked) {
        sprite.setColor(sf::Color(120, 255, 120));
    }
}

void Door::unlock() {
    if (doorKind == DoorKind::Victory) return;
    if (!locked) return;

    locked = false;
    sprite.setColor(sf::Color(120, 255, 120));

    if (doorKind == DoorKind::BossGate) {
        if (RunScoreTracker* tracker = RunScoreTracker::active()) {
            tracker->onBossGateUnlocked();
        }
        EventBus::instance().emit("boss_gate_unlocked");
    } else {
        EventBus::instance().emit("door_unlock");
    }
}

bool Door::isLocked() const {
    return locked;
}

bool Door::canPass() const {
    return !locked;
}

void Door::onInteract(Player& player) {
    if (doorKind == DoorKind::Victory) return;

    if (doorKind == DoorKind::BossGate) {
        if (locked) {
            if (player.getKeys() < Constants::KEYS_REQUIRED_FOR_BOSS) return;
            for (int i = 0; i < Constants::KEYS_REQUIRED_FOR_BOSS; ++i) {
                player.useKey();
            }
            unlock();
        } else {
            EventBus::instance().emit("enter_boss_arena");
        }
        return;
    }

    if (!locked) return;

    if (player.getKeys() <= 0) return;
    if (player.useKey()) {
        unlock();
    }
}

void Door::update(float dt) {
    if (!locked || doorKind == DoorKind::Victory) return;

    static float timer = 0.f;
    timer += dt;
    float glow = 200.f + std::sin(timer * 4.f) * 55.f;

    if (doorKind == DoorKind::BossGate) {
        sprite.setColor(sf::Color(255.f, glow * 0.75f, glow * 0.25f));
    } else {
        sprite.setColor(sf::Color(glow, glow * 0.6f, glow * 0.6f));
    }
}
