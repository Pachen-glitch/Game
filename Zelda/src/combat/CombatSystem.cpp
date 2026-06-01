#include "CombatSystem.h"

#include <algorithm>

#include <SFML/Graphics/Rect.hpp>

#include "../core/Constants.h"
#include "../entity/enemy/Enemy.h"
#include "../entity/enemy/NarutoProjectile.h"
#include "../entity/items/Heart.h"
#include "../entity/player/Direction.h"
#include "../interaction/EventBus.h"
#include "../utils/MathUtils.h"

#include <cstdlib>

namespace {

bool dropsHeartOnDeath(const Enemy& enemy) {
    switch (enemy.getKind()) {
        case EnemyKind::Slime:
        case EnemyKind::Skeleton:
        case EnemyKind::Summoner:
            return std::rand() % 100 < 25;
        default:
            return false;
    }
}

} // namespace
// Limpia los hitboxes
void CombatSystem::clear() {
    hitboxes.clear();
}
// Añade un hitbox
void CombatSystem::addHitbox(const Hitbox& box) {
    hitboxes.push_back(box);
}
// Devuelve el rectangulo de offset
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
// Spawns un hitbox de espada
void CombatSystem::spawnSwordHitbox(Player& player) {
    sf::Vector2f offset;
    sf::Vector2f hbSize(36.f, 28.f);

    switch (player.getFacingDirection()) {
        case Direction::UP:    offset = {6.f, -28.f}; break;
        case Direction::DOWN:  offset = {6.f, 40.f}; break;
        case Direction::LEFT:  offset = {-32.f, 8.f}; break;
        case Direction::RIGHT: offset = {40.f, 8.f}; break;
    }
// Crea un hitbox de espada
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
// Spawns un hitbox de spin
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
// Actualiza los hitboxes
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
    resolveProjectileHits(player, entities);
}
// Orienta los proyectiles de Naruto
void CombatSystem::steerNarutoProjectiles(
    float dt,
    Player& player,
    EntityManager& entities
) {
    sf::Vector2f targetCenter =
        player.getPosition() + player.getSize() * 0.5f;

    for (auto& ent : entities.all()) { // Itera sobre todos los proyectiles
        if (!ent || !ent->isActive()) continue;
        if (ent->getType() != EntityType::Projectile) continue;

        auto* proj = dynamic_cast<NarutoProjectile*>(ent.get()); // Convierte el ent a NarutoProjectile
        if (!proj) continue;

        proj->applyHomingSteer(targetCenter, dt); // Orienta el proyectil
    }
}
// Resuelve los hits del jugador
void CombatSystem::resolvePlayerHits(Player& player, EntityManager& entities) {
    (void)player;
    std::vector<sf::Vector2f> pendingHearts;

    for (const auto& hb : hitboxes) {
        if (!hb.fromPlayer) continue;

        for (auto& ent : entities.all()) {
            if (!ent || !ent->isActive()) continue;
            if (ent->getType() != EntityType::Enemy) continue;

            auto* enemy = dynamic_cast<Enemy*>(ent.get());
            if (!enemy || enemy->isDead() || enemy->isDeathAnimPending()) continue;
            if (enemy->isHurtAnimating()) continue;

            if (hb.rect.intersects(enemy->getBounds())) {
                const bool wasAlive = !enemy->isDeathAnimPending();
                const sf::Vector2f deathPos = enemy->getPosition();
                enemy->takeHit(static_cast<int>(hb.damage), hb.knockback);

                if (wasAlive && enemy->isDeathAnimPending() &&
                    dropsHeartOnDeath(*enemy)) {
                    pendingHearts.push_back(
                        deathPos + sf::Vector2f(8.f, 8.f)
                    );
                }
            }
        }
    }

    for (const sf::Vector2f& pos : pendingHearts) {
        entities.spawn<Heart>(pos);
    }
}
// Resuelve los hits del enemigo
void CombatSystem::resolveEnemyHits(Player& player, EntityManager& entities) {
    if (!player.canTakeDamage()) return;

    for (auto& ent : entities.all()) {
        if (!ent || !ent->isActive()) continue;
        if (ent->getType() != EntityType::Enemy) continue;

        auto* enemy = dynamic_cast<Enemy*>(ent.get()); // Convierte el ent a Enemy
        if (!enemy || enemy->isDead()) continue; // Si el enemigo esta muerto, no se resuelve el hit
        if (!enemy->canDealContactDamage()) continue;

        if (!enemy->getContactBounds().intersects(player.getBounds())) continue; // Si el hitbox del enemigo no intersecta con el hitbox del jugador, no se resuelve el hit

        player.damage(enemy->getContactDamage());

        // Empuje en direccion opuesta al slime (alejando al jugador).
        sf::Vector2f kb = MathUtils::directionTo(
            enemy->getPosition(), player.getPosition()
        ) * enemy->getContactKnockback();
        player.applyKnockback(kb);

        enemy->resetContactCooldown(0.85f);
    }
}

void CombatSystem::resolveProjectileHits(Player& player, EntityManager& entities) {
    if (!player.canTakeDamage()) return;

    for (auto& ent : entities.all()) {
        if (!ent || !ent->isActive()) continue;
        if (ent->getType() != EntityType::Projectile) continue;

        auto* proj = dynamic_cast<NarutoProjectile*>(ent.get());
        if (!proj) continue;
        if (!proj->getBounds().intersects(player.getBounds())) continue; // Si el hitbox del proyectil no intersecta con el hitbox del jugador, no se resuelve el hit

        player.damage(proj->getDamage()); // Hace daño al jugador

        sf::Vector2f kb = MathUtils::normalize(
            player.getPosition() - proj->getPosition()
        ) * proj->getKnockback();
        player.applyKnockback(kb);

        EventBus::instance().emit("player_damaged");
        proj->deactivate();
    }
}
