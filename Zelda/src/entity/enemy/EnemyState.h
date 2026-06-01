#pragma once

enum class EnemyState {
    Idle,
    Wander,
    Chase,
    Attack,
    ReturnToSpawn,
    Hurt,
    CallAllies,
    Dead
};

enum class EnemyKind {
    Slime,
    Bat,
    Skeleton,
    Summoner,
    Naruto,
    NarutoClone
};
