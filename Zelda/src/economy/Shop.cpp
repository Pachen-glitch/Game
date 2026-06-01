#include "Shop.h"
#include "../entity/player/Player.h"
#include "../save/RunScoreTracker.h"

#include <cstdlib>

void Shop::randomize(int seed) {
    std::srand(seed);
    items.clear();

    items.push_back({
        "Heart +1",
        50,
        [](Player& p) { p.increaseMaxHearts(1.f); }
    });

    items.push_back({
        "Damage Up",
        80,
        [](Player& p) { p.getStats().swordDamage += 0.5f; }
    });

    items.push_back({
        "Speed Up",
        60,
        [](Player& p) { p.getStats().moveSpeedMult += 0.1f; }
    });

    items.push_back({
        "Attack Speed",
        70,
        [](Player& p) { p.getStats().attackSpeedMult += 0.15f; }
    });

    items.push_back({
        "Max Life",
        100,
        [](Player& p) { p.increaseMaxHearts(1.f); }
    });
}

bool Shop::buy(Player& player, int index) {
    if (index < 0 || index >= static_cast<int>(items.size())) return false;
    auto& item = items[index];
    if (player.getStats().rupees < item.cost) return false;

    player.getStats().rupees -= item.cost;
    if (item.apply) item.apply(player);
    if (RunScoreTracker* tracker = RunScoreTracker::active()) {
        tracker->onShopPurchase(item.cost);
    }
    return true;
}
