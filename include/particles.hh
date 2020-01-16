#pragma once

class ParticleRandomizer
{
    static ParticleRandomizer *mInstance;

    ParticleRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for ParticleRandomizer.
    static ParticleRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();
};
