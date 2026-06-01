#pragma once

// Naruto boss attack identifiers — used by AI + NarutoRenderer.
enum class NarutoAttack { // Identificadores de ataques del boss Naruto
    None,
    Run,
    Kick,
    Dash,
    Retreat,
    Circle,
    CloneSummon,
    OdamaRasengan,
    RasenShuriken,
    Substitution,
    DefensiveSubstitution
};

enum class NarutoBossPhase {
    One,
    Two,
    Three
};
