#pragma once

// Naruto boss attack identifiers — used by AI + NarutoRenderer.
enum class NarutoAttack {
    None,
    Kick,
    Dash,
    Retreat,
    Circle,
    CloneSummon,
    OdamaRasengan,
    RasenShuriken,
    Substitution
};

enum class NarutoBossPhase {
    One,
    Two,
    Three
};
