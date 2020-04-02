
#pragma once

class RespawnPointRandomizer
{
    static RespawnPointRandomizer *mInstance;

    RespawnPointRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for RespawnPointRandomizer.
    static RespawnPointRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();
};
