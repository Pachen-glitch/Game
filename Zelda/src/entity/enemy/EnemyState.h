#pragma once

enum class EnemyState {
    Idle,
    Patrol,
    Chase,
    Attack,
    CallAllies,
    Dead
};

enum class EnemyKind {
    Slime,
    Bat,
    Skeleton,
    Summoner
};
