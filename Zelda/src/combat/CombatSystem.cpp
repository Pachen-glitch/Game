#include "CombatSystem.h"

#include <algorithm>

#include <SFML/Graphics/Rect.hpp>

#include "../core/Constants.h"
#include "../entity/enemy/Enemy.h"
#include "../entity/player/Direction.h"
#include "../interaction/EventBus.h"
#include "../utils/MathUtils.h"

void CombatSystem::clear() {
    hitboxes.clear();
}

void CombatSystem::addHitbox(const Hitbox& box) {
    hitboxes.push_back(box);
}

sf::FloatRect CombatSystem::offsetRect(
    sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset
) {
    return sf::FloatRect(
        pos.x + offset.x,
        pos.y + offset.y,
        size.x,
        size.y
    );
}

void CombatSystem::spawnSwordHitbox(Player& player) {
    sf::Vector2f offset;
    sf::Vector2f hbSize(36.f, 28.f);

    switch (player.getDirection()) {
        case Direction::UP:    offset = {6.f, -28.f}; break;
        case Direction::DOWN:  offset = {6.f, 40.f}; break;
        case Direction::LEFT:  offset = {-32.f, 8.f}; break;
        case Direction::RIGHT: offset = {40.f, 8.f}; break;
    }

    Hitbox hb;
    hb.rect = offsetRect(player.getPosition(), hbSize, offset);
    hb.damage = player.getSwordDamage();
    hb.knockback = MathUtils::normalize(
        sf::Vector2f(hb.rect.left + hb.rect.width * 0.5f,
                     hb.rect.top + hb.rect.height * 0.5f) -
        (player.getPosition() + player.getSize() * 0.5f)
    ) * Constants::KNOCKBACK_FORCE;
    hb.lifetime = Constants::ATTACK_DURATION * 0.6f;
    hb.fromPlayer = true;
    hb.ownerId = nextOwnerId++;
    addHitbox(hb);
}

void CombatSystem::spawnSpinHitbox(Player& player) {
    sf::Vector2f center = player.getPosition() + player.getSize() * 0.5f;
    float r = 56.f;

    Hitbox hb;
    hb.rect = sf::FloatRect(center.x - r, center.y - r, r * 2.f, r * 2.f);
    hb.damage = player.getSwordDamage() * 1.5f;
    hb.lifetime = Constants::SPIN_DURATION * 0.7f;
    hb.fromPlayer = true;
    hb.ownerId = nextOwnerId++;
    addHitbox(hb);
}

void CombatSystem::update(float dt, Player& player, EntityManager& entities) {
    for (auto& hb : hitboxes) {
        hb.tick(dt);
    }
    hitboxes.erase(
        std::remove_if(hitboxes.begin(), hitboxes.end(),
            [](const Hitbox& h) { return h.isExpired(); }),
        hitboxes.end()
    );

    resolvePlayerHits(player, entities);
    resolveEnemyHits(player, entities);
}

void CombatSystem::resolvePlayerHits(Player& player, EntityManager& entities) {
    for (const auto& hb : hitboxes) {
        if (!hb.fromPlayer) continue;

        for (auto& ent : entities.all()) {
            if (!ent || !ent->isActive()) continue;
            if (ent->getType() != EntityType::Enemy) continue;

            auto* enemy = dynamic_cast<Enemy*>(ent.get());
            if (!enemy || enemy->isDead()) continue;

            if (hb.rect.intersects(enemy->getBounds())) {
                enemy->takeHit(static_cast<int>(hb.damage), hb.knockback);
            }
        }
    }
}

void CombatSystem::resolveEnemyHits(Player& player, EntityManager& entities) {
    if (!player.canTakeDamage()) return;

    for (auto& ent : entities.all()) {
        if (!ent || !ent->isActive()) continue;
        if (ent->getType() != EntityType::Enemy) continue;

        auto* enemy = dynamic_cast<Enemy*>(ent.get());
        if (!enemy || enemy->isDead()) continue;

        if (enemy->getAttackBounds().intersects(player.getBounds())) {
            player.damage(1);
            sf::Vector2f kb = MathUtils::directionTo(
                enemy->getPosition(), player.getPosition()
            ) * Constants::KNOCKBACK_FORCE;
            player.applyKnockback(kb);
        }
    }
}
