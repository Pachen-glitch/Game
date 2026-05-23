# The Legend of Pachen — Engine Manual & Architecture Documentation

**Version:** 1.0  
**Language:** C++17  
**Framework:** SFML 2.6  
**Project:** Zelda-inspired procedural roguelike  
**Audience:** All developers (onboarding, gameplay, rendering, procedural, systems)  
**Status:** Living document — update when adding systems

---

## Table of Contents

1. [Introduction & Design Philosophy](#1-introduction--design-philosophy)
2. [Repository Layout](#2-repository-layout)
3. [Boot Sequence & Entry Points](#3-boot-sequence--entry-points)
4. [C++ Concepts Used in This Codebase](#4-c-concepts-used-in-this-codebase)
5. [Architecture Layers & Communication](#5-architecture-layers--communication)
6. [The Frame Pipeline (Update Loop)](#6-the-frame-pipeline-update-loop)
7. [Core Layer](#7-core-layer)
8. [Entity Layer](#8-entity-layer)
9. [Movement Layer](#9-movement-layer)
10. [Combat Layer](#10-combat-layer)
11. [Animation & Render Pipeline](#11-animation--render-pipeline)
12. [Procedural Generation Layer](#12-procedural-generation-layer)
13. [Economy & Save Layer](#13-economy--save-layer)
14. [UI Layer](#14-ui-layer)
15. [Interaction & EventBus](#15-interaction--eventbus)
16. [Audio Layer](#16-audio-layer)
17. [Utils & Asset Pipeline](#17-utils--asset-pipeline)
18. [Threading, Mutexes & Semaphores](#18-threading-mutexes--semaphores)
19. [Enemy AI Guide (Carlos)](#19-enemy-ai-guide-carlos)
20. [Procedural Design Guide (Daniel)](#20-procedural-design-guide-daniel)
21. [Gameplay Loops](#21-gameplay-loops)
22. [Future Roadmap & Extension Points](#22-future-roadmap--extension-points)
23. [Anti-Patterns & Architecture Rules](#23-anti-patterns--architecture-rules)
24. [Complete Class Reference](#24-complete-class-reference)
25. [Glossary](#25-glossary)

---

## 1. Introduction & Design Philosophy

### What This Engine Is

**The Legend of Pachen** is a modular 2D action RPG engine built for:

- **Room-based exploration** (Zelda: A Link to the Past / Minish Cap feel)
- **Roguelike pacing** (Isaac-style room flow, permadeath runs with meta-save)
- **State-driven gameplay** (player and enemy behavior expressed as explicit states)
- **Event-driven decoupling** (systems communicate through `EventBus`, not direct calls everywhere)

### Why Modular Architecture?

| Problem | Solution in this engine |
|---------|-------------------------|
| Game logic mixed with rendering | Renderers only *display* state; they never decide gameplay |
| Hardcoded asset paths | `AssetPaths` scans and resolves paths at startup |
| Texture loading every frame | `TextureCache` singleton loads once, reuses forever |
| Systems tightly coupled | `GameSession` orchestrates; systems don't own each other |
| Feature creep breaking combat | `CombatSystem`, `CombatFeel`, `Hitbox` are isolated |
| Procedural content chaos | `DungeonGenerator` → `Room` graph → `World` population |

### Design Principles (Non-Negotiable)

1. **Thin entry, fat systems** — `main.cpp` and `Game.cpp` do almost nothing.
2. **Gameplay in systems/entities, not renderers** — `PlayerRenderer` reads `PlayerState`; it does not attack.
3. **Data flows down, events flow sideways** — `GameSession` passes `dt` and references; `EventBus` broadcasts signals.
4. **Extend via inheritance + composition** — new enemies inherit `Enemy`; new rooms extend `RoomType` handling in `World`.
5. **Constants in one place** — tune `Constants.h`, not magic numbers in 20 files.
6. **No spaghetti in `GameSession`** — if a loop gets complex, extract a system, don't inline logic.

### Who Should Read What

| Role | Start here |
|------|------------|
| New programmer | Sections 4, 6, 8, 24 |
| Gameplay / combat | Sections 10, 19, 21 |
| Rendering / animation | Sections 11, 24 (render/*) |
| Procedural / level design | Sections 12, 20 |
| Economy / meta | Sections 13, 22 |
| Systems / performance | Sections 18, 22 |

---

## 2. Repository Layout

```
Game/Zelda/
├── assets/                 # Sprites, fonts, audio (copied to build output)
├── build/                  # CMake build directory
├── docs/
│   └── ENGINE_MANUAL.md    # ← This document
├── src/
│   ├── animation/          # Animation clips & AnimationManager
│   ├── audio/              # AudioManager (stubbed for DLL safety)
│   ├── combat/             # Hitboxes, CombatSystem, CombatFeel
│   ├── core/               # Game loop, constants, game states
│   ├── economy/            # Shop, EconomySystem
│   ├── entity/             # All gameplay entities
│   │   ├── base/           # Entity, EntityManager
│   │   ├── player/         # Player, stats, states
│   │   ├── enemy/          # AI, enemy types
│   │   ├── items/          # Pickups, doors
│   │   └── npc/            # Shopkeeper
│   ├── input/              # Legacy console input (deprecated)
│   ├── interaction/        # EventBus, InteractionSystem
│   ├── map/                # Tile grid Map
│   ├── movement/           # Movement, collision
│   ├── procedural/         # Dungeon, rooms, World
│   ├── render/             # All drawing (no gameplay decisions)
│   ├── save/               # SaveSystem, SaveData
│   ├── threads/            # JobQueue, ThreadPool, AsyncSemaphore
│   ├── ui/                 # HUD, minimap, shop UI, screens
│   └── utils/              # AssetPaths, Timer, MathUtils, FontLoader
├── CMakeLists.txt
└── main.cpp
```

---

## 3. Boot Sequence & Entry Points

### Flow Diagram

```
main()
  ├── showMenu()          [console: start / instructions / exit]
  └── runGame()
        └── GameSession::run()
              ├── AssetPaths::initialize()
              ├── AssetPaths::validateCriticalAssets()
              ├── SFML window creation
              ├── setupEvents()      [EventBus subscriptions]
              ├── SaveSystem::load()
              ├── World::newRun()
              └── while (window.isOpen()) { ... frame pipeline ... }
```

### `main.cpp`

- **Responsibility:** Process entry, optional boot logging, invoke console menu, call `runGame()`.
- **Why separate from GameSession:** Keeps platform/bootstrap concerns out of the game loop.

### `Game.cpp` → `runGame()`

- **Responsibility:** Single line of indirection: `GameSession session; session.run();`
- **Why:** Allows future swapping (headless server, editor mode) without touching `main`.

### `GameSession::run()`

- **Responsibility:** **Orchestrator only.** Owns the frame loop, constructs systems on the stack, wires events, never implements combat math or AI directly.
- **Depends on:** Every major system header.
- **Used by:** `Game.cpp` only.

---

## 4. C++ Concepts Used in This Codebase

This section teaches the C++ you will read daily in this project.

### References (`&`)

```cpp
void MovementSystem::update(float dt, Player& player, const Map& map);
```

- `Player& player` — **reference** to existing player; no copy, modifications affect the real object.
- `const Map& map` — read-only reference; collision won't mutate the map.

**Why:** Entities and systems are large; passing by value would copy sprites, stats, timers every frame.

### `const`

- **On parameters:** "I will not modify this."
- **On methods:** `float getHealth() const` — safe to call on const objects.
- **On constexpr (Constants.h):** compile-time constants, zero runtime cost.

### Inheritance & Polymorphism

```cpp
class Enemy : public Entity { ... };
class SlimeEnemy : public Enemy { ... };
```

- `Entity` defines interface: `update()`, `onInteract()`.
- `EntityManager` stores `std::unique_ptr<Entity>` but spawns concrete types.
- **`virtual`** on base methods allows `SlimeEnemy::think()` to override behavior.
- **`override`** on derived methods catches signature mistakes at compile time.

**Why:** One loop can update coins, slimes, and player — each runs its own logic.

### `dynamic_cast`

```cpp
auto* enemy = dynamic_cast<Enemy*>(e.get());
```

- Safely converts `Entity*` → `Enemy*` at runtime.
- Returns `nullptr` if entity is not an enemy.

**Why:** EntityManager is type-agnostic; combat and AI need specific types.

### `std::unique_ptr`

```cpp
std::vector<std::unique_ptr<Entity>> entities;
```

- **Unique ownership:** when removed, entity is destroyed automatically.
- **No manual delete** — prevents leaks when clearing rooms.

### `std::vector`

- Dynamic array for hitboxes, rooms, animation frames, event handlers.
- Cache-friendly iteration in hot loops.

### `std::unordered_map`

- Used in `EventBus` (event → handlers), `TextureCache` (path → texture), `EntityRenderer` (slime → animator).
- O(1) average lookup by string or pointer key.

**Why:** Fast lookup for assets and per-enemy animation state.

### `std::function` & Lambdas

```cpp
bus.subscribe("enemy_hit", []() {
    CombatFeel::instance().triggerHitPause(0.05f);
});
```

- **Callback** stored and invoked later on `emit()`.
- Shop items use `std::function<void(Player&)>` for purchase effects.

### Singleton Pattern (`instance()`)

```cpp
EventBus& EventBus::instance() {
    static EventBus bus;
    return bus;
}
```

- One global instance (Meyers singleton — thread-safe in C++11+).
- Used for: `EventBus`, `TextureCache`, `CombatFeel`, `AudioManager`.

**Why:** Systems need global access without passing pointers through 15 constructors.

**Caution:** Don't singleton everything — gameplay objects (Player, Enemies) are NOT singletons.

### Delta Time (`dt`)

```cpp
float dt = clock.restart().asSeconds();
player.update(dt);
```

- Time elapsed since last frame in **seconds**.
- Movement: `position += velocity * dt` — frame-rate independent.
- At 60 FPS, `dt ≈ 0.0167`.

### `enum class`

```cpp
enum class PlayerState { Idle, Walk, Attack, ... };
enum class EnemyState { Idle, Wander, Chase, ... };
```

- Strongly typed enums — no accidental `int` confusion.
- Used for state machines everywhere.

### `std::move`

- Transfers ownership without copying heavy objects (jobs in `JobQueue`).
- Used in thread pool job submission.

### Static Utility Classes

`CollisionSystem`, `EnemyMovement`, `EnemyAI`, `FontLoader` — classes with only static methods, no instances.

**Why:** Group related functions without global namespace pollution.

---

## 5. Architecture Layers & Communication

### Layer Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                        core/ GameSession                     │
│              (orchestrates frame, owns no gameplay rules)    │
└────────────┬────────────────────────────────────────────────┘
             │
    ┌────────┼────────┬──────────┬──────────┬──────────┐
    ▼        ▼        ▼          ▼          ▼          ▼
 movement/ combat/  entity/  procedural/ interaction/ economy/
    │        │        │          │          │          │
    └────────┴────────┴────┬─────┴──────────┴──────────┘
                           │
              ┌────────────┼────────────┐
              ▼            ▼            ▼
          animation/    render/        ui/
              │            │
              └───── utils/ (AssetPaths, TextureCache)
                           │
                      threads/ (background work)
```

### How Layers Talk

| From | To | Mechanism |
|------|-----|-----------|
| Combat | Audio | `EventBus::emit("enemy_died")` |
| Player | Combat | State change detected in GameSession → spawn hitbox |
| Enemy | CombatFeel | Direct call on hit (short-lived singleton) |
| Shopkeeper | UI | `EventBus::emit("open_shop")` |
| World | EntityManager | `entities.spawn<SlimeEnemy>()` |
| Renderers | Entities | Read-only: position, state, velocity |
| Movement | Map | `CollisionSystem::resolveMovement` |
| Any | Assets | `AssetPaths` + `TextureCache` |

### What Must NOT Happen

- `PlayerRenderer` calling `player.damage()`
- `EnemyAnimator` running AI or pathfinding
- `DungeonGenerator` spawning SFML sprites
- Hardcoded `"assets/sprites/player/idle/idle_d1.png"` in 10 files

---

## 6. The Frame Pipeline (Update Loop)

This is the exact order inside `GameSession::run()` when `GameState::Playing`:

```
1. Poll SFML events (close, ESC, E interact, R retry)
2. CombatFeel::tick(dt)
3. gameDt = dt * CombatFeel::getTimeScale()   ← hit-pause slow-mo
4. MovementSystem::update(gameDt, player, map)
      ├── Shield (V), Attack (F), Spin (X)
      ├── WASD → velocity + direction
      └── CollisionSystem resolves position
5. If attack mid-swing → CombatSystem::spawnSwordHitbox()
6. If spin started → CombatSystem::spawnSpinHitbox()
7. Player::update(gameDt)                     ← state timers, knockback
8. World::updateEnemies(player, gameDt, map)  ← AI think + movement
9. EntityManager::updateAll(gameDt)           ← entity timers
10. CombatSystem::update(gameDt, ...)         ← hitbox tick + resolve hits
11. InteractionSystem::update()               ← auto-pickup radius
12. World::tryTransition()                    ← door → loadRoom
13. Check GameOver / Victory
14. PlayerRenderer::update(gameDt)
15. EntityRenderer::update(gameDt)
16. Camera::update(gameDt)
17. Draw: tilemap → entities → damage numbers
18. Draw UI: HUD, minimap, overlays
19. window.display()
```

### Input → Output Data Flow

```
Keyboard
  → MovementSystem sets Player.velocity, Player.direction, PlayerState
  → CollisionSystem writes Player.position
  → PlayerRenderer reads PlayerState + direction → AnimationManager clip
  → Camera reads Player.position → sf::View
  → HUD reads PlayerStats → sf::Text
```

### Combat Data Flow

```
F key
  → Player::trySwordAttack() → PlayerState::Attack, lockedAttackDirection
  → (mid-attack) CombatSystem::spawnSwordHitbox()
  → Hitbox.rect intersects Enemy.bounds
  → Enemy::takeHit() → health, knockback, Hurt state, EventBus
  → CombatFeel::triggerHitPause()
  → EnemyAnimator shows hurt/death clip + flash tint
```

---

## 7. Core Layer

### `Constants` (namespace)

Central tuning: tile size (48px), room size (20×20), speeds, combat timings, aggro radii, window size, camera lerp.

**Scalability:** Balance passes edit one file.  
**Performance:** `constexpr` — no runtime lookup.

### `GameState` (enum class)

`Playing | Paused | Shop | GameOver | Victory`

Controls which update/draw branches run. High-level FSM for application, separate from `PlayerState`.

### `CombatFeel` (singleton)

| Variable | Purpose |
|----------|---------|
| `hitPauseRemaining` | Brief slow-mo after hits |
| `enemyFlashRemaining` | Window for hurt flash tint |

| Method | Purpose |
|--------|---------|
| `triggerHitPause()` | Extends pause if new hit is longer |
| `getTimeScale()` | Returns 0.08 during pause, else 1.0 |
| `setEnemyFlashTimer()` | Triggers red tint on hurt enemies |

**Gameplay role:** "Game juice" — makes combat feel impactful without changing damage numbers.

**Performance:** Trivial — two floats per frame.

### `GameSession`

See Section 6. Extension point: extract `PlayingStateController` if loop grows.

### Legacy: `Menu`, `Pause` (console)

ASCII console screens from early prototype. SFML uses `ui/GameScreens` and `ui/Menu` instead.

---

## 8. Entity Layer

### Design: Entity Component Pattern (Lite)

We use **inheritance polymorphism**, not full ECS:

- **Entity** = position, size, sprite, type, active flag
- **Specialized behavior** = virtual methods on subclasses

**Why not full ECS?** Team size and scope favor readable OOP for Zelda-like entities. ECS can come later for 500+ entities.

### `Entity` (abstract base)

| Aspect | Detail |
|--------|--------|
| **Responsibility** | Common spatial presence + sprite + lifecycle |
| **Purpose** | Uniform storage in EntityManager |
| **Depends on** | TextureCache (load sprite on construct) |
| **Used by** | All world objects |

**Key members:** `position`, `size`, `sprite`, `spritePath`, `active`, `entityType`

**Key methods:**
- `update(dt)` — pure virtual per-type tick
- `onInteract(Player&)` — pure virtual pickup/NPC/door behavior
- `getBounds()` — AABB for collision/combat
- `deactivate()` — soft destroy (waits for death anim)

**Lifecycle:**
```
spawn → active=true → update each frame → deactivate → removeInactive()
```

### `EntityManager`

| Method | Purpose |
|--------|---------|
| `spawn<T>(args...)` | `make_unique<T>`, push to vector |
| `updateAll(dt)` | Calls each entity's update |
| `removeInactive()` | Erases where `!isActive()` |
| `clear()` | Room transition — destroy all |

**Ownership:** `unique_ptr` — manager owns entities.

**Scalability:** For 100+ entities, consider spatial hash for queries. Current linear scan is fine for room scale (~20 entities).

### `Player`

See Section 10 for combat. Movement states: Idle, Walk, Attack, Spin, Shield, Hurt, Dead.

**Important timers:**
- `attackTimer` — attack animation lock duration
- `swordCooldown` — prevents spam
- `invulnTimer` — i-frames after damage
- `hurtTimer` — knockback lockout

**`lockedAttackDirection`:** Direction frozen at attack start — hitbox and animation stay consistent.

### `PlayerStats`

Numeric layer separated from Entity — keeps `Player.h` readable.

Fields: hearts, rupees, keys, swordDamage, speed multipliers, unlock flags.

### Enemy Hierarchy

```
Entity
 └── Enemy
      ├── SlimeEnemy    (hop AI, death split hook)
      ├── BatEnemy      (fast, wobble)
      ├── SkeletonEnemy (CallAllies on hurt)
      └── SummonerEnemy (spawn slimes, kite)
```

### Items & NPCs

| Class | Gameplay |
|-------|----------|
| `Coin` | +1 rupee, deactivate |
| `Heart` | +1 heart |
| `Key` | +1 key |
| `Door` | Locked until key used |
| `Shopkeeper` | Emits `open_shop` |

---

## 9. Movement Layer

### `MovementSystem`

**Single owner of player input.** Reads SFML keyboard directly.

| Key | Action |
|-----|--------|
| WASD | Move |
| F | Sword attack |
| X | Spin attack |
| V | Shield hold |

Edge-trigger flags (`attackPressed`) prevent holding key = machine gun attacks.

**Does NOT run when:** `!player.canMove()` (attack, spin, hurt, dead).

### `CollisionSystem`

Axis-separated resolution (slide along walls):

1. Try X movement
2. Try Y movement
3. Corner tests via four AABB points vs `TileType::WALL`

**Performance:** O(1) per entity per frame — only tests nearby tiles via integer division.

### `EnemyMovement`

Wraps collision + **room clamping** — enemies cannot leave map bounds (1 tile margin).

Used by `Enemy::applyMovement()` after AI sets velocity.

### `PlayerMovement` (legacy)

Tile-step discrete movement — **deprecated** in favor of smooth `MovementSystem`.

---

## 10. Combat Layer

### Philosophy

Combat is **hitbox-driven**, not animation-driven:

- Animations show intent
- Hitboxes deal damage at precise timing
- Separation allows tuning damage without re-exporting sprites

### `Hitbox` struct

| Field | Role |
|-------|------|
| `rect` | World-space damage area |
| `damage` | Float damage amount |
| `knockback` | Vector applied to target |
| `lifetime` / `age` | Auto-expire |
| `fromPlayer` | Player vs enemy origin |
| `ownerId` | Future: prevent multi-hit |

### `CombatSystem`

**Spawn methods:**
- `spawnSwordHitbox` — directional offset by `player.getFacingDirection()`
- `spawnSpinHitbox` — circular AOE, 1.5× damage

**Resolve methods:**
- `resolvePlayerHits` — hitbox vs enemy bounds → `takeHit()`
- `resolveEnemyHits` — contact damage + knockback + cooldown

### Combat Feel Stack

```
Hit connects
  → Enemy::takeHit(knockback)
  → CombatFeel::triggerHitPause(0.05s)
  → CombatFeel::setEnemyFlashTimer()
  → EventBus::emit("enemy_hit")
  → EnemyState::Hurt + hurtAnimTimer
  → Enemy velocity = knockback * 1.35
  → Player invulnTimer on receiving damage
```

### i-Frames (Invulnerability)

- Player: `invulnTimer` after damage — `canTakeDamage()` checks it
- Shield: blocks damage entirely when active
- Enemy: brief hurt state skips re-hit during `hurtAnimTimer`

### Attack Timing

Sword hitbox spawns at `Constants::ATTACK_HIT_FRAME_TIME` (0.09s into swing), not on frame 1 — aligns damage with visible sword frame.

---

## 11. Animation & Render Pipeline

### Separation Rule

> **Renderers are translators:** Gameplay state → visual state. Nothing else.

### Pipeline Stages

```
AssetPaths (discover PNG paths)
    ↓
Animation (clip = frames + durations)
    ↓
AnimationManager (named clips, active clip, tick)
    ↓
TextureCache (path → sf::Texture)
    ↓
sf::Sprite (position, scale, color tint)
    ↓
sf::RenderWindow::draw()
```

### `TextureCache`

**Why cache?** Loading PNG from disk every frame = stutter + disk thrashing.

- First `get(path)` loads and stores
- Missing files → magenta fallback + log once
- `failedPaths` prevents retry spam

### `Animation` / `AnimationManager`

- **Animation:** ordered frames, per-frame duration, loop flag
- **AnimationManager:** map of clip names → play/update/applyToSprite

**Player clips:** `idle_down`, `walk_up`, `attack_left`, etc.  
**Slime clips:** `idle`, `walk`, `hurt`, `death`

### Animation Priority (Player)

```
Dead > Hurt > Attack > Walk > Idle
```

Implemented in `PlayerRenderer::clipForState()`.

### `PlayerRenderer`

- Builds clips from `AssetPaths` at construction (once)
- Each frame: pick clip from state → update anim → apply sprite to player

### `EnemyAnimator`

- One instance per slime (map in EntityRenderer)
- `pickClip()` reads AI state + velocity magnitude
- Death clip completion → `enemy.deactivate()`

### `EntityRenderer`

- Updates slime animators
- Draw order: player sprite, then entities

### `TilemapRenderer`

Colored rectangles per tile type — placeholder until tile sprites. Fast, no texture binding.

### `Camera`

Exponential lerp toward player center:  
`position += (target - position) * CAMERA_LERP * dt`

Smooth Zelda-style follow without snapping.

---

## 12. Procedural Generation Layer

### `RoomType`

| Type | Purpose |
|------|---------|
| Start | Tutorial pacing, guaranteed pickups |
| Combat | Enemy arenas |
| Treasure | Rupees, keys, hearts |
| Shop | Shopkeeper NPC |
| Boss | Final encounter |
| Corridor | Low-threat connector (future pacing) |

### `Room`

- Owns `Map` grid
- `generateLayout()` — walls, door gaps, optional pillars
- `connections` — graph edges with `DoorSide`, lock flag
- `visited`, `cleared` — minimap + progression

### `DungeonGenerator`

**Algorithm:** BFS frontier expansion on 2D grid

1. Pick room count (MIN_ROOMS–MAX_ROOMS)
2. Place rooms on grid with 45% expansion chance per direction
3. Assign types: first=Start, last=Boss, weighted middle
4. `connectRooms` — primarily linear South/North chain (Isaac/Zelda hybrid)
5. ~20% doors locked

**Why not maze?** Zelda rooms are **arenas**, not labyrinth corridors. Linear backbone + branches = predictable pacing.

### `World`

**Run state owner:**
- `newRun(seed)` → generate dungeon, load room 0
- `loadRoom(id)` → clear entities, populate by type
- `tryTransition` — proximity to door → load connected room

**Population table (simplified):**

| RoomType | Spawns |
|----------|--------|
| Start | 2 slimes, heart, key |
| Combat/Boss | 2–3 slimes, random pickups |
| Treasure | key, heart, 3 coins |
| Shop | shopkeeper |

---

## 13. Economy & Save Layer

### `Shop` + `ShopItem`

Items are `{ name, cost, std::function<void(Player&)> apply }`.

`randomize(seed)` builds 5 items with lambdas modifying stats.

### `EconomySystem`

Facade over single `Shop` — `openShop(player, seed)` re-rolls inventory.

### `SaveSystem`

Plain-text `save.dat` — meta progression between runs:

- Persists stat upgrades, unlock flags
- `applyToPlayer` on run start
- `captureFromPlayer` on exit

**Architectural rule:** Run state (current room, entities) is NOT saved mid-run yet — future feature goes in `SaveData` extension.

---

## 14. UI Layer

| Class | Role |
|-------|------|
| `HUD` | Hearts, rupees, keys (screen space) |
| `Minimap` | Room grid, visited/current/boss/shop colors |
| `ShopUI` | Purchase panel, numpad 1–3 |
| `DamageNumbers` | Floating combat text |
| `GameScreens` | Pause, game over, victory overlays |

**Render order:** World (view transform) → reset to default view → HUD/minimap → modal overlays.

**Event-driven UI:** Shop opens on `"open_shop"` event, not direct Shopkeeper → ShopUI call.

---

## 15. Interaction & EventBus

### `EventBus`

Pub/sub with string event names:

```cpp
EventBus::instance().subscribe("enemy_died", []() { ... });
EventBus::instance().emit("enemy_died");
```

**Known events:**

| Event | Typical subscribers |
|-------|---------------------|
| `player_attack` | AudioManager |
| `player_damaged` | AudioManager |
| `enemy_hit` | CombatFeel |
| `enemy_died` | AudioManager |
| `rupee_pickup` | AudioManager |
| `heart_pickup` | AudioManager |
| `open_shop` | GameSession (state change) |

**Scalability:** Replace strings with enum + typed payloads when event count grows.

### `InteractionSystem`

- Auto-pickup within 36px (skips enemies)
- Manual E interact within 64px (nearest entity)
- Calls `Entity::onInteract(player)`

---

## 16. Audio Layer

### `AudioManager` (stub)

SFML-audio removed from link to prevent missing DLL crash on Windows.

Interface preserved: `playSound(name)`, `playMusic(path)`.

**Future:** Re-enable with bundled DLLs; load via background thread (see Section 18).

---

## 17. Utils & Asset Pipeline

### `AssetPaths`

**Startup:**
1. Find executable directory
2. Locate `assets/` folder
3. Recursively index all PNGs
4. Build sorted frame lists by prefix (`walk_d1`, `walk_d2`, ...)

**Why scan instead of hardcode?** Artists add frames without code changes.

### `Timer`

Countdown for AI pauses, attack cooldowns, hitbox lifetime, i-frames.

### `MathUtils`

`distance`, `normalize`, `directionTo` — shared vector math.

### `FontLoader`

Tries multiple paths once; UI classes share pattern: load font in constructor.

---

## 18. Threading, Mutexes & Semaphores

### Current State

The **main thread** owns:

- SFML window
- Input polling
- Gameplay update
- Rendering

Background infrastructure exists in `src/threads/`:

| Class | Purpose |
|-------|---------|
| `JobQueue` | Mutex-protected FIFO of `std::function<void()>` |
| `ThreadPool` | Worker threads consuming JobQueue |
| `AsyncSemaphore` | Counting semaphore for backpressure |

### Why Multithreading Exists

| Task | Thread? | Reason |
|------|---------|--------|
| Main game loop | Main only | SFML context is not thread-safe |
| Dungeon generation | Background | Can take 5–50ms for large graphs |
| Texture preloading | Background | Avoid frame hitch on room load |
| Enemy pathfinding | Background | A* on 20×20 grid — batch across enemies |
| Audio decode | Background | Disk I/O stalls main thread |
| Room prefetch | Background | Generate next room while player fights |

### What Is Dangerous to Share

| Data | Risk | Protection |
|------|------|------------|
| `EntityManager` | Race on spawn/destroy | Main thread only for mutations |
| `sf::Texture` / `sf::Sprite` | SFML thread safety | Load on worker, **assign on main** |
| `TextureCache` map | Concurrent insert | `std::mutex` around get/insert |
| `EventBus` | Handler list mutation | Mutex or main-thread-only emit |
| `World::rooms` | Read during write | Generate in buffer, swap on main |

### Race Conditions

Happens when two threads read/write same memory without synchronization:

```
Thread A: enemy.health -= 1
Thread B: enemy.health -= 1
Result: health wrong (lost update)
```

**Fix:** mutex, atomic, or single-thread ownership.

### Mutex vs Semaphore

| Tool | Meaning | Use in this engine |
|------|---------|---------------------|
| **Mutex** | Only one thread in critical section | TextureCache insert, JobQueue push/pop |
| **Semaphore** | N slots available | Limit 2 pending room generations; prefetch pool |

### C++ Synchronization Primitives

```cpp
std::mutex m;
{
    std::lock_guard<std::mutex> lock(m);  // auto-unlock at scope end
    textures[path] = loadedTexture;
}

std::condition_variable cv;  // JobQueue: wait for jobs
std::atomic<int> count;      // lock-free counters for pending jobs
std::thread worker([] { ... });
```

### Safe Integration Pattern

```
Background thread:
  1. Generate Room data (tiles only, no EntityManager)
  2. Push result to thread-safe queue
  3. Signal semaphore

Main thread (end of frame):
  1. Drain queue
  2. Merge prefetched rooms into World
  3. Spawn entities via EntityManager
```

### Planned Integration Points

| System | File | Integration |
|--------|------|-------------|
| Async dungeon | `World::newRun` | `ThreadPool::submit(generate)` |
| Texture warmup | `TextureCache::get` | Preload job per room type |
| AI batch | `World::updateEnemies` | Optional parallel think (read-only map) |
| Audio | `AudioManager::load` | Worker loads buffers, main registers |

### Rules

1. **Never draw from worker thread**
2. **Never mutate EntityManager from worker**
3. **Keep critical sections tiny** — copy data out, process outside lock
4. **Prefer job results over shared state**

---

## 19. Enemy AI Guide (Carlos)

### Architecture Overview

```
Enemy::think(player, dt, map)     ← decision + velocity
    ↓
modifyVelocity(dt)                ← per-type feel (Slime hops)
    ↓
EnemyMovement::moveWithCollision  ← physics
    ↓
EnemyAnimator::pickClip           ← visuals only
```

`EnemyAI` class exists for shared helpers but **primary AI lives in `Enemy.cpp`**.

### `EnemyState` Machine

```
        ┌──────────┐
        │   Idle   │◄────────────────┐
        └────┬─────┘                 │
             │ timer expires         │
             ▼                       │
        ┌──────────┐                 │
        │  Wander  │                 │
        └────┬─────┘                 │
             │ sees player           │ deaggro / home reached
             ▼                       │
        ┌──────────┐    too far     │
        │  Chase   │──────────────► ReturnToSpawn
        └────┬─────┘                 │
             │ in range              │
             ▼                       │
        ┌──────────┐                 │
        │  Attack  │─────────────────┘
        └──────────┘

Hurt interrupts movement (velocity decay)
Dead → deathAnimPending → EnemyAnimator → deactivate
```

### Wander Logic

1. **Idle:** velocity = 0, wait `stateTimer` (0.7–2.0s random)
2. **Pick direction:** `pickWanderDirection(map)` — tries 4 cardinals, skips walls
3. **Wander:** move at 55% speed for 0.28–0.65s
4. Return to Idle

**Design intent:** SNES Zelda enemies pause, step, pause — not constant sliding.

### Aggro Logic

```cpp
if (seesPlayer(player)) aggroMemory.start(aggroMemoryDuration);
bool hasAggro = !aggroMemory.finished();
bool inChaseRange = distPlayer <= deaggroRadius;
bool tooFarFromHome = distSpawn > maxChaseFromSpawn;
```

- **aggroRadius:** detection (per-type)
- **aggroMemoryDuration:** chase after losing line-of-sight
- **deaggroRadius:** give up if player runs far
- **maxChaseFromSpawn:** leash — triggers ReturnToSpawn

### Slime Hopping

`SlimeEnemy::modifyVelocity`:

- Outside hop burst → velocity zeroed
- Chase always allows movement
- Timers: `hopCooldown`, `hopBurstTimer`

**Tuning:** `initKindStats()` — moveSpeed 52, chaseSpeed 68, short aggro 120.

### Adding a New Enemy (Checklist)

1. Create `MyEnemy.h/.cpp` inheriting `Enemy`
2. Override `initKindStats()` — speeds, aggro, damage
3. Optionally override `think()` or `modifyVelocity()`
4. Register sprite paths via `AssetPaths::getEnemyAnimFrames`
5. Add animator support in `EntityRenderer` (or generic EnemyAnimator)
6. Spawn in `World::populateRoom()` by room type
7. **Do NOT** put AI in renderer

### Ranged Enemy Pattern (Future)

```
EnemyState::Attack
  → if attackCooldown ready
  → spawn Projectile entity via EntityManager
  → Projectile::update moves hitbox
  → CombatSystem resolves separately
```

New class: `entity/projectile/Projectile.cpp`  
Extend: `CombatSystem` with `fromPlayer = false` hitboxes

### Boss Architecture (Future)

```
BossEnemy : Enemy
  ├── Phase 1: normal chase
  ├── Phase 2: below 50% HP → new attack pattern table
  └── BossPatternController (composition, not in renderer)
         ├── patternCooldown
         ├── std::vector<std::function<void(Boss&, Player&)>>
         └── telegraph → hitbox spawn timing
```

Room: `RoomType::Boss` already exists — spawn boss in `populateRoom`.

### Summoner Reference

`SummonerEnemy` demonstrates:

- External `EntityManager*` for spawning adds
- Kite behavior when player too close
- `CallAllies` state on summon

---

## 20. Procedural Design Guide (Daniel)

### Design Philosophy

| Zelda | Isaac | This engine |
|-------|-------|-------------|
| Hand-crafted room shape | Random rooms | Procedural layout + typed content |
| Locked progression | Branching map | Linear spine + grid positions |
| Combat arenas | Enemy waves | 2–3 enemies per combat room |

**Not a labyrinth:** players should understand where to go. Minimap shows grid topology.

### Room Flow

```
Start → Combat → [Treasure/Shop branch] → ... → Boss
```

`DungeonGenerator::connectRooms` creates South/North links between sequential IDs. Grid positions enable minimap layout.

### Pacing Levers

| Lever | Location |
|-------|----------|
| Room count | `Constants::MIN_ROOMS`, `MAX_ROOMS` |
| Shop guarantee | `pickWeightedRoom` + `shopPlaced` flag |
| Treasure guarantee | `treasurePlaced` flag |
| Locked doors | 20% in `connectRooms` |
| Enemy count | `World::populateRoom` switch |
| Pickup density | random rolls in populateRoom |

### Adding a New Room Type

1. Add to `RoomType` enum
2. Handle in `DungeonGenerator::pickWeightedRoom`
3. Implement layout rules in `Room::generateLayout` (optional)
4. Add population case in `World::populateRoom`
5. Add minimap color in `Minimap::draw`
6. Document pacing intent in design notes

### Event Room Example (Future)

```cpp
case RoomType::Event:
    // No enemies — spawn NPC, choice altar, curse shrine
    entities.spawn<EventNPC>(...);
    break;
```

### Shop Room

Already implemented: spawns `Shopkeeper`, triggers `"open_shop"` on interact.

### Treasure Room

High pickup density — rewards exploration branches.

### Boss Room

Last index in generation — populate with boss enemy type when implemented.

### Async Generation (Future)

Use `ThreadPool` to generate room `N+1` while player in room `N`. Main thread swaps when entering door.

---

## 21. Gameplay Loops

### Exploration Loop

```
Enter room → fight/collect → find door → transition → mark visited on minimap
```

### Combat Loop

```
Spot enemy → aggro → attack (F) / dodge / shield (V)
  → hit feedback (pause, flash, knockback)
  → pickup drops → heal/rearm
```

### Economy Loop

```
Collect rupees → find shop room → buy stat upgrades
  → stronger combat → deeper dungeon
```

### Meta Loop

```
Run ends → SaveSystem captures best stats → next run starts with upgrades
```

### System Connections

```
Keys ──► Door unlock ──► new rooms
Rupees ──► Shop ──► PlayerStats
Hearts ──► sustain combat
Enemies ──► room clearance (future) ──► doors open
```

---

## 22. Future Roadmap & Extension Points

| Feature | Where it goes | Communicates via |
|---------|---------------|------------------|
| Bosses | `entity/enemy/BossEnemy` | World populate, EventBus |
| Projectiles | `entity/projectile/` | CombatSystem hitboxes |
| Potions | `entity/items/Potion`, Shop items | PlayerStats, Interaction |
| Chests | `entity/items/Chest` | Interaction, loot tables |
| Elemental damage | `PlayerStats` + `DamageType` enum | CombatSystem |
| Status effects | `StatusEffectComponent` on Entity | CombatSystem tick |
| Rare loot | `LootTable` in procedural | World populate |
| Skill tree | `meta/SkillTree` + SaveData | SaveSystem |
| Co-op | Second Player in EntityManager | MovementSystem input map |
| Async loading | threads/ + World prefetch | JobQueue results |
| Dynamic events | `RoomType::Event` | World, EventBus |

### Extension Rules

1. Add enum/state before adding behavior
2. Emit events for cross-system reactions
3. Keep new logic out of renderers
4. Add constants before tuning in cpp files

---

## 23. Anti-Patterns & Architecture Rules

### DO

- Extend `Enemy` for new monsters
- Register animations in renderer constructor
- Use `EventBus` for audio/UI reactions
- Pass `const Map&` into AI think
- Use `Timer` for cooldowns

### DO NOT

- Load textures every frame
- Hardcode asset paths in gameplay code
- Put AI inside `EnemyAnimator`
- Add 500 lines to `GameSession.cpp`
- Make every system a singleton
- Call SFML draw from worker threads

---

## 24. Complete Class Reference

Below: condensed 12-point reference for every class. Expand in source headers as needed.

### core/

#### `CombatFeel`
1. **Responsibility:** Hit-pause and flash timing  
2. **Purpose:** Combat game juice  
3. **Why:** Centralize feel without coupling combat to render  
4. **Used by:** GameSession, Enemy::takeHit, EnemyAnimator  
5. **Depends on:** None  
6. **Variables:** hitPauseRemaining, enemyFlashRemaining  
7. **Methods:** tick, triggerHitPause, getTimeScale, setEnemyFlashTimer  
8. **Logic:** Time scale 0.08 during pause  
9. **Gameplay:** SNES-style impact  
10. **Scale:** Add screen shake here  
11. **Perf:** Negligible  
12. **Extend:** Combo meter, slow-mo tiers  

#### `GameSession`
1. **Responsibility:** Main loop orchestration  
2. **Purpose:** Wire systems without owning rules  
3. **Why:** Single place for frame order  
4. **Used by:** Game.cpp  
5. **Depends on:** All systems  
6. **Variables:** Local stack systems in run()  
7. **Methods:** run(), setupEvents()  
8. **Logic:** See Section 6  
9. **Gameplay:** Enables all interaction  
10. **Scale:** Split state handlers  
11. **Perf:** O(entities) per frame  
12. **Extend:** Add GameState handlers map  

### entity/base/

#### `Entity`
1. **Responsibility:** Spatial game object base  
2. **Purpose:** Polymorphic container  
3. **Why:** Uniform EntityManager storage  
4. **Used by:** Everything in world  
5. **Depends on:** TextureCache  
6. **Variables:** position, size, sprite, active, entityType  
7. **Methods:** update, onInteract, getBounds, deactivate  
8. **Logic:** Constructs sprite from path  
9. **Gameplay:** Physical presence  
10. **Scale:** Optional component arrays  
11. **Perf:** One texture lookup at spawn  
12. **Extend:** Team/faction id  

#### `EntityManager`
1. **Responsibility:** Own and iterate entities  
2. **Purpose:** Room-scoped entity list  
3. **Why:** Central spawn/destroy  
4. **Used by:** World, Combat, Interaction  
5. **Depends on:** Entity hierarchy  
6. **Variables:** vector<unique_ptr<Entity>>  
7. **Methods:** spawn, updateAll, removeInactive, clear  
8. **Logic:** Template spawn forwards constructor args  
9. **Gameplay:** Room population  
10. **Scale:** Spatial partitioning  
11. **Perf:** Linear scan OK for room scale  
12. **Extend:** Query by type/radius  

### entity/player/

#### `Player`
1. **Responsibility:** Player gameplay entity  
2. **Purpose:** Input response + combat states  
3. **Why:** Separate from renderer  
4. **Used by:** Movement, Combat, UI, Save  
5. **Depends on:** PlayerStats, Timer, EventBus  
6. **Variables:** state, direction, velocity, timers  
7. **Methods:** trySwordAttack, canMove, damage, heal  
8. **Logic:** State timers gate transitions  
9. **Gameplay:** Player avatar  
10. **Scale:** Equipment slots  
11. **Perf:** O(1)  
12. **Extend:** New PlayerState values  

#### `PlayerStats`
1. **Responsibility:** Numeric stats  
2. **Purpose:** Keep Player lean  
3. **Why:** Shop/save modify stats cleanly  
4. **Used by:** Player, HUD, Shop, Save  
5. **Depends on:** None  
6. **Variables:** hearts, rupees, keys, multipliers  
7. **Methods:** heal, takeDamage, isDead  
8. **Logic:** Float hearts for half-damage  
9. **Gameplay:** Progression  
10. **Scale:** Status effect list  
11. **Perf:** Trivial  
12. **Extend:** Element resistances  

### entity/enemy/

#### `Enemy`
1. **Responsibility:** Base AI state machine  
2. **Purpose:** Reusable enemy logic  
3. **Why:** DRY for all monster types  
4. **Used by:** World, Combat, EnemyAnimator  
5. **Depends on:** Map, CombatFeel, Timers  
6. **Variables:** aiState, velocity, spawnPosition, aggro timers  
7. **Methods:** think, takeHit, applyMovement  
8. **Logic:** See Section 19  
9. **Gameplay:** Room challenge  
10. **Scale:** Behavior trees later  
11. **Perf:** One think per enemy per frame  
12. **Extend:** override initKindStats, modifyVelocity  

#### `SlimeEnemy` / `BatEnemy` / `SkeletonEnemy` / `SummonerEnemy`
See Section 19 for behavior. Each overrides stats and selective methods.

#### `EnemyAI`
1. **Responsibility:** Static AI helpers  
2. **Purpose:** Shared math utilities  
3. **Why:** Optional factoring from Enemy  
4. **Used by:** Minimal — prefer Enemy methods  
5. **Note:** Candidate for merge or expansion for pathfinding  

### combat/

#### `CombatSystem`
1. **Responsibility:** Hitbox lifecycle + resolution  
2. **Purpose:** Central combat authority  
3. **Why:** Single place for damage rules  
4. **Used by:** GameSession  
5. **Depends on:** Hitbox, EntityManager, Player  
6. **Variables:** hitboxes vector, nextOwnerId  
7. **Methods:** spawnSwordHitbox, update, resolve*  
8. **Logic:** AABB intersection  
9. **Gameplay:** All damage dealing  
10. **Scale:** Hit groups, friendly fire  
11. **Perf:** O(hitboxes × entities) — small sets  
12. **Extend:** Projectile hitboxes  

#### `Hitbox`
Struct — timed damage rect. See Section 10.

### movement/

#### `MovementSystem` — Section 9  
#### `CollisionSystem` — Section 9  
#### `EnemyMovement` — Section 9  

### animation/

#### `Animation`
1. **Responsibility:** Single clip playback  
2. **Purpose:** Frame sequence over time  
3. **Why:** Separate from manager  
4. **Used by:** AnimationManager, AssetPaths  
5. **Variables:** frames, currentIndex, timer, looping  
6. **Methods:** update, getCurrentFramePath, isFinished  

#### `AnimationManager`
1. **Responsibility:** Named clip registry + active clip  
2. **Purpose:** Drive sprites  
3. **Why:** Entities need multiple clips  
4. **Used by:** PlayerRenderer, EnemyAnimator  
5. **Variables:** clips map, activeName  
6. **Methods:** registerAnimation, play, applyToSprite  

### render/

#### `TextureCache` — Section 11  
#### `PlayerRenderer` — Section 11  
#### `EnemyAnimator` — Section 11  
#### `EntityRenderer` — Section 11  
#### `TilemapRenderer` — Section 11  
#### `Camera` — Section 11  

### procedural/

#### `Room` — Section 12  
#### `DungeonGenerator` — Section 12  
#### `World` — Section 12  

### economy/

#### `Shop` / `ShopItem` / `EconomySystem` — Section 13  

### save/

#### `SaveSystem` / `SaveData` — Section 13  

### ui/

#### `HUD` / `Minimap` / `ShopUI` / `DamageNumbers` / `GameScreens` — Section 14  

### interaction/

#### `EventBus` — Section 15  
#### `InteractionSystem` — Section 15  

### audio/

#### `AudioManager` — Section 16 (stub)  

### utils/

#### `AssetPaths` — Section 17  
#### `Timer` — Section 17  
#### `MathUtils` — Section 17  
#### `FontLoader` — Section 17  

### threads/

#### `JobQueue`
1. **Responsibility:** Thread-safe job FIFO  
2. **Purpose:** Decouple work submission from execution  
3. **Why:** Safe background processing  
4. **Used by:** ThreadPool  
5. **Depends on:** mutex, condition_variable  
6. **Variables:** jobs deque, shutdown flag  
7. **Methods:** push, pop, shutdown  

#### `ThreadPool`
1. **Responsibility:** Fixed worker threads  
2. **Purpose:** Execute jobs off main thread  
3. **Why:** Room gen / preload without blocking  
4. **Used by:** Future World, TextureCache warmup  
5. **Methods:** submit, waitIdle, pendingJobs  

#### `AsyncSemaphore`
1. **Responsibility:** Counting semaphore  
2. **Purpose:** Limit concurrent background tasks  
3. **Why:** Prevent memory spike from 50 prefetch rooms  
4. **Methods:** acquire, release, tryAcquire  

### map/

#### `Map`
1. **Responsibility:** 2D tile grid  
2. **Purpose:** Collision + rendering source  
3. **Why:** Rooms share tile abstraction  
4. **Used by:** Room, CollisionSystem, Enemy AI  
5. **Variables:** grid vector<vector<TileType>>  

### Items & NPCs

#### `Coin`, `Heart`, `Key`, `Door`, `Shopkeeper`
See Section 8 — thin Entity subclasses with `onInteract` implementations.

---

## 25. Glossary

| Term | Meaning |
|------|---------|
| **dt** | Delta time — seconds since last frame |
| **i-frames** | Invulnerability frames after damage |
| **Aggro** | Enemy awareness/chase of player |
| **Hitbox** | Damage-dealing area |
| **Hurtbox** | Vulnerable area (here: entity bounds) |
| **Leash** | Max distance enemy chases from spawn |
| **FSM** | Finite State Machine (Idle, Chase, ...) |
| **Pub/Sub** | EventBus pattern |
| **Roguelike run** | Single attempt from start to death/victory |
| **Meta-save** | Progress persisting between runs |

---

## Document Maintenance

When you add a system:

1. Update this file's class reference
2. Add event names to Section 15
3. Update frame pipeline if order changes
4. Add tuning constants to Section 7
5. Notify team in commit message: `docs: update ENGINE_MANUAL for X`

---

## Appendix A — Expanded Per-Class Reference (Full 12-Point Format)

This appendix documents **every type** in the engine with the full 12-point template requested for internal onboarding.

---

### `Entity` (`entity/base/Entity.h`)

| # | Detail |
|---|--------|
| 1. Responsibility | Abstract base for all objects that exist in the game world with position and sprite. |
| 2. Purpose | Provides a uniform interface so `EntityManager` can store heterogeneous objects in one container. |
| 3. Why it exists | Without a common base, combat and interaction would need separate lists for enemies, items, and NPCs — duplication and sync bugs. |
| 4. Used by | EntityManager, CombatSystem, InteractionSystem, all renderers, World. |
| 5. Depends on | SFML Sprite/Rect, TextureCache, forward-declared Player. |
| 6. Variables | `position`, `size`, `sprite`, `spritePath`, `active`, `entityType`. |
| 7. Methods | Pure virtual `update(dt)`, `onInteract(player)`; concrete getters/setters for transform and lifecycle. |
| 8. Internal logic | Constructor loads texture once via cache; `getBounds()` returns FloatRect from position+size. |
| 9. Gameplay role | Everything the player can touch, fight, or collect is an Entity. |
| 10. Scalability | Future: optional `EntityId` for networking; component flags without breaking hierarchy. |
| 11. Performance | Texture loaded once at spawn — never per frame. |
| 12. Extension | Subclass + register in World populate; add `EntityType` enum value if needed for queries. |

---

### `EntityManager` (`entity/base/EntityManager.h`)

| # | Detail |
|---|--------|
| 1. Responsibility | Own, spawn, update, and destroy dynamic entities for the active room. |
| 2. Purpose | Single authoritative list for everything that isn't the tile map. |
| 3. Why it exists | Room transitions require clearing all entities atomically — one `clear()` call. |
| 4. Used by | World, CombatSystem, InteractionSystem, SummonerEnemy (spawn adds). |
| 5. Depends on | Entity hierarchy, `<memory>`, `<vector>`. |
| 6. Variables | `std::vector<std::unique_ptr<Entity>> entities`. |
| 7. Methods | `spawn<T>(...)`, `updateAll(dt)`, `removeInactive()`, `all()`, `clear()`. |
| 8. Internal logic | Template spawn uses `std::make_unique<T>`; removeInactive uses erase-remove idiom. |
| 9. Gameplay role | When you enter a room, everything you fight is here. |
| 10. Scalability | At 100+ entities add `queryRadius()` with spatial hash; keep spawn API stable. |
| 11. Performance | Linear iteration — acceptable for ~30 entities per room. |
| 12. Extension | Add `forEachType(EntityType, callback)` without exposing raw vector. |

---

### `Player` (`entity/player/Player.h`)

| # | Detail |
|---|--------|
| 1. Responsibility | Player avatar: movement state, combat actions, inventory hooks, damage rules. |
| 2. Purpose | Central gameplay object the entire engine orbits around. |
| 3. Why it exists | Separating Player from PlayerRenderer allows simulation without SFML window (headless tests). |
| 4. Used by | MovementSystem, CombatSystem, InteractionSystem, HUD, SaveSystem, Camera, all UI. |
| 5. Depends on | Entity, PlayerStats, Timer, Constants, EventBus, AssetPaths. |
| 6. Variables | `stats`, `state`, `direction`, `lockedAttackDirection`, `velocity`, five Timers, combat flags. |
| 7. Methods | Combat: `trySwordAttack`, `trySpinAttack`, `shouldSpawnSwordHit`; movement: `canMove`, `updateLocomotionState`; economy: `addRupees`, `heal`. |
| 8. Internal logic | `update()` applies velocity during attack/spin/hurt with decay; timers auto-reset states to Idle. |
| 9. Gameplay role | The hero — all player-facing mechanics live here or in PlayerStats. |
| 10. Scalability | New weapons = new PlayerState + CombatSystem spawn method. |
| 11. Performance | O(1) per frame; no allocations in hot path. |
| 12. Extension | Add `PlayerState::Roll` — block movement in MovementSystem, add iframe timer. |

---

### `Enemy` (`entity/enemy/Enemy.h`)

| # | Detail |
|---|--------|
| 1. Responsibility | Generic enemy AI: wander, chase, attack, return home, hurt, death. |
| 2. Purpose | 80% of enemy behavior shared across types — DRY. |
| 3. Why it exists | Copy-pasting AI into each enemy file creates inconsistent aggro and unmaintainable tuning. |
| 4. Used by | World::updateEnemies, CombatSystem, EnemyAnimator. |
| 5. Depends on | Map, CollisionSystem, EnemyMovement, CombatFeel, EventBus, Timer, MathUtils. |
| 6. Variables | `aiState`, `health`, `velocity`, `spawnPosition`, `wanderDirection`, aggro/contact timers, speed/radius tuning. |
| 7. Methods | `think(player, dt, map)`, `takeHit`, protected AI helpers, virtual `initKindStats`, `modifyVelocity`. |
| 8. Internal logic | State selection then switch; velocity integrated through collision; hurt/death short-circuit movement. |
| 9. Gameplay role | Room challenge and contact damage threat. |
| 10. Scalability | Behavior composition (patrol routes, formations) can wrap think(). |
| 11. Performance | One collision resolve per enemy per frame. |
| 12. Extension | Override `modifyVelocity` for feel; override `think` only when state machine differs entirely. |

---

### `SlimeEnemy` (`entity/enemy/SlimeEnemy.h`)

| # | Detail |
|---|--------|
| 1. Responsibility | Slow, hop-based slime with short aggro and organic pauses. |
| 2. Purpose | Default enemy — teaches combat, feels alive not robotic. |
| 3. Why it exists | Base wander is smooth; slimes need burst movement to match sprite animation. |
| 4. Used by | World populate, Summoner spawn, death split hook. |
| 5. Depends on | Enemy, AssetPaths, EntityManager (split). |
| 6. Variables | `hopBurstActive`, `hopCooldown`, `hopBurstTimer`. |
| 7. Methods | `think`, `modifyVelocity`, `initKindStats`, static `onDeathSplit`. |
| 8. Internal logic | Hop timers gate velocity to zero between bursts; calls base think for decisions. |
| 9. Gameplay role | First combat encounter, low damage contact (0.5 heart). |
| 10. Scalability | Size variants = different HP/speed in constructor. |
| 11. Performance | Same as Enemy. |
| 12. Extension | Elemental slime = override takeHit + tint in animator. |

---

### `CombatSystem` (`combat/CombatSystem.h`)

| # | Detail |
|---|--------|
| 1. Responsibility | Authoritative damage resolution via hitboxes and contact rules. |
| 2. Purpose | Single source of truth for "did this attack hit?" |
| 3. Why it exists | Damage in Player or Enemy classes creates double-hit bugs and inconsistent knockback. |
| 4. Used by | GameSession exclusively for update/spawn. |
| 5. Depends on | Hitbox, Player, EntityManager, Enemy, Constants, MathUtils. |
| 6. Variables | `std::vector<Hitbox> hitboxes`, `nextOwnerId`. |
| 7. Methods | `spawnSwordHitbox`, `spawnSpinHitbox`, `update`, `resolvePlayerHits`, `resolveEnemyHits`. |
| 8. Internal logic | Tick hitbox ages; AABB intersection; skip hurt enemies on re-hit. |
| 9. Gameplay role | Every sword swing and spin damage goes through here. |
| 10. Scalability | Add hit tracking per ownerId; damage types; team filters. |
| 11. Performance | Small N×M — room scale only. |
| 12. Extension | `spawnProjectileHitbox` for ranged — same struct, different spawn site. |

---

### `DungeonGenerator` (`procedural/DungeonGenerator.h`)

| # | Detail |
|---|--------|
| 1. Responsibility | Build dungeon graph: room count, types, grid positions, connections. |
| 2. Purpose | Procedural replayability without hand-building every floor. |
| 3. Why it exists | Manual level design doesn't scale for roguelike runs. |
| 4. Used by | World::newRun. |
| 5. Depends on | Room, RoomType, Constants, queue/set for BFS. |
| 6. Variables | `seed` — drives deterministic randomness. |
| 7. Methods | `generate(seed)`, private `pickWeightedRoom`, `connectRooms`. |
| 8. Internal logic | BFS frontier expansion; weighted type roll; linear South/North wiring. |
| 9. Gameplay role | Defines run structure — pacing, shop chance, boss placement. |
| 10. Scalability | ThreadPool generate → return vector; branch connections beyond linear. |
| 11. Performance | Runs once per run — not per frame. |
| 12. Extension | Add `RoomType::Secret` with low weight roll in pickWeightedRoom. |

---

### `World` (`procedural/World.h`)

| # | Detail |
|---|--------|
| 1. Responsibility | Active run: rooms, current room id, entity population, transitions. |
| 2. Purpose | Bridge between dungeon data and live gameplay. |
| 3. Why it exists | GameSession shouldn't know spawn tables or door logic. |
| 4. Used by | GameSession (primary), minimap via getRooms(). |
| 5. Depends on | DungeonGenerator, EntityManager, all entity types, Map. |
| 6. Variables | `rooms`, `currentRoomId`, `entities`, `generator`. |
| 7. Methods | `newRun`, `loadRoom`, `populateRoom`, `updateEnemies`, `tryTransition`. |
| 8. Internal logic | loadRoom clears entities and repopulates; transition by door proximity. |
| 9. Gameplay role | The dungeon as experienced moment-to-moment. |
| 10. Scalability | Prefetch next room via ThreadPool; lazy entity spawn zones. |
| 11. Performance | populateRoom O(n) spawns — once per room entry. |
| 12. Extension | Room clear flag → open doors; boss defeat handler. |

---

### `AssetPaths` (`utils/AssetPaths.h`)

| # | Detail |
|---|--------|
| 1. Responsibility | Discover, index, and resolve all asset paths relative to executable. |
| 2. Purpose | Eliminate hardcoded sprite paths and support dynamic frame counts. |
| 3. Why it exists | Artists rename/add frames — code shouldn't break. |
| 4. Used by | Entity constructors, PlayerRenderer, EnemyAnimator, FontLoader, AudioManager (future). |
| 5. Depends on | Filesystem, Animation builder, Direction/Enemy enums. |
| 6. Variables (internal) | `gKnownRelative`, `gRelToAbs`, resolve cache, missing log set. |
| 7. Methods | `initialize`, `collectFramesByPrefix`, `getPlayer*Frames`, `getEnemyAnimFrames`, `buildAnimation`. |
| 8. Internal logic | Recursive PNG scan; numeric sort on frame suffix; prefix matching. |
| 9. Gameplay role | Invisible — enables all visuals to load correctly. |
| 10. Scalability | Async scan on ThreadPool; hot-reload in dev builds. |
| 11. Performance | Scan once at startup — O(total PNG count). |
| 12. Extension | Add `getProjectileFrames(type)` following same prefix pattern. |

---

### `EventBus` (`interaction/EventBus.h`)

| # | Detail |
|---|--------|
| 1. Responsibility | String-keyed publish/subscribe event dispatch. |
| 2. Purpose | Decouple systems that react to events without direct pointers. |
| 3. Why it exists | Shopkeeper shouldn't include ShopUI header — emit event instead. |
| 4. Used by | Player, Enemy, Shopkeeper, GameSession setupEvents. |
| 5. Depends on | `<functional>`, `<unordered_map>`, `<vector>`. |
| 6. Variables | `handlers` map: event name → vector of callbacks. |
| 7. Methods | `subscribe`, `emit`, `clear`, `instance`. |
| 8. Internal logic | emit iterates all handlers for key — synchronous, same thread. |
| 9. Gameplay role | Audio/UI reactions to combat and economy. |
| 10. Scalability | Typed events with payload struct; mutex if emitting from workers (dispatch to main queue). |
| 11. Performance | O(handlers per event) — keep handlers lightweight. |
| 12. Extension | `subscribeOnce`, priority ordering, debug event log. |

---

### `JobQueue` / `ThreadPool` / `AsyncSemaphore` (`threads/`)

Documented in Section 18. These types form the **concurrency foundation**:

- **JobQueue** protects shared job deque with `std::mutex` + `condition_variable`.
- **ThreadPool** owns N worker `std::thread`s pulling from JobQueue.
- **AsyncSemaphore** limits concurrent prefetch slots (e.g. max 2 rooms generating).

**Integration status:** Infrastructure ready; main loop still single-threaded for SFML safety. Next step: `World::prefetchRoom(id)` submitting to ThreadPool.

---

### Remaining Classes — Quick Full Reference

The following classes follow the same architectural rules documented above. Location indicates where to read source.

| Class | File | One-line responsibility |
|-------|------|-------------------------|
| `PlayerStats` | entity/player/PlayerStats.h | Numeric stats struct |
| `PlayerRenderer` | render/PlayerRenderer.h | Player animation → sprite |
| `EnemyAnimator` | render/EnemyAnimator.h | Slime animation → sprite |
| `EntityRenderer` | render/EntityRenderer.h | Batch entity draw + slime anim map |
| `TextureCache` | render/TextureCache.h | Path → texture singleton |
| `Animation` | animation/Animation.h | Frame sequence player |
| `AnimationManager` | animation/AnimationManager.h | Named clip registry |
| `Camera` | render/Camera.h | Smoothed follow view |
| `TilemapRenderer` | render/TilemapRenderer.h | Map debug draw |
| `MovementSystem` | movement/MovementSystem.h | Player input + collision |
| `CollisionSystem` | movement/CollisionSystem.h | Tile AABB resolution |
| `EnemyMovement` | movement/EnemyMovement.h | Enemy move + clamp |
| `CombatFeel` | core/CombatFeel.h | Hit pause + flash timing |
| `Hitbox` | combat/Hitbox.h | Damage rectangle struct |
| `Room` | procedural/Room.h | Single dungeon cell |
| `Shop` | economy/Shop.h | Item inventory + buy |
| `ShopItem` | economy/ShopItem.h | Purchasable entry + callback |
| `EconomySystem` | economy/EconomySystem.h | Shop facade |
| `SaveSystem` | save/SaveSystem.h | Meta save/load |
| `SaveData` | save/SaveSystem.h | Persisted struct |
| `HUD` | ui/HUD.h | Stats overlay |
| `Minimap` | ui/Minimap.h | Room grid overlay |
| `ShopUI` | ui/ShopUI.h | Shop panel + input |
| `DamageNumbers` | ui/DamageNumbers.h | Floating damage text |
| `GameScreens` | ui/GameScreens.h | Pause/game over/victory |
| `InteractionSystem` | interaction/InteractionSystem.h | Pickup + interact |
| `AudioManager` | audio/AudioManager.h | Sound stub |
| `Map` | map/Map.h | Tile grid |
| `Timer` | utils/Timer.h | Countdown utility |
| `MathUtils` | utils/MathUtils.h | Vector math |
| `FontLoader` | utils/FontLoader.h | Font path resolver |
| `BatEnemy` | entity/enemy/BatEnemy.h | Fast wobble flyer |
| `SkeletonEnemy` | entity/enemy/SkeletonEnemy.h | Tank + CallAllies on hit |
| `SummonerEnemy` | entity/enemy/SummonerEnemy.h | Spawns slimes, kites |
| `EnemyAI` | entity/enemy/EnemyAI.h | Static AI helpers |
| `Coin/Heart/Key/Door` | entity/items/*.h | Interactable pickups |
| `Shopkeeper` | entity/npc/Shopkeeper.h | Shop trigger NPC |
| `PlayerMovement` | movement/PlayerMovement.h | Legacy tile step |
| `GameSession` | core/GameSession.h | Main loop |
| `Game` | core/Game.h | runGame() wrapper |

For any class not expanded to 12 rows above, use the **Section 24 template** when modifying that class — copy the table and fill in during code review.

---

## Appendix B — Controls & Constants Quick Reference

| Input | Action |
|-------|--------|
| WASD | Move |
| F | Sword attack |
| X | Spin attack |
| V | Shield (hold) |
| E | Interact |
| ESC | Pause / close shop |
| R | Retry (game over) |

| Constant | Value | Meaning |
|----------|-------|---------|
| TILE_SIZE | 48 | Pixels per tile |
| PLAYER_SPEED | 200 | Base move speed |
| SWORD_COOLDOWN | 0.42s | Attack cooldown |
| ATTACK_HIT_FRAME_TIME | 0.09s | When hitbox spawns |
| HIT_PAUSE_DURATION | 0.05s | Combat freeze feel |
| AGGRO_RADIUS | 160 | Default detect range |

---

## Appendix C — Collaboration Workflow

### Adding a Feature (Team Checklist)

1. **Design** — Which layer? Which existing class extends?
2. **Constants** — Add tuning to `Constants.h`
3. **Implement** — System/entity only; no renderer logic
4. **Events** — Cross-system reactions via EventBus
5. **Assets** — Drop PNGs in correct folder; run game to validate log
6. **Docs** — Update this manual (Section 24 + appendix)
7. **Review** — Verify frame pipeline order unchanged or documented

### Code Review Questions

- Does this class know too much about other layers?
- Are textures loaded through AssetPaths + TextureCache?
- Can this run without SFML window (where applicable)?
- If using threads: is shared data mutex-protected?
- Are new magic numbers in Constants.h?

---

*End of Engine Manual — The Legend of Pachen*
