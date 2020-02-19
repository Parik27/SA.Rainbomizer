#pragma once

struct tHandlingData;

class HandlingRandomizer
{
    static HandlingRandomizer *mInstance;

    tHandlingData *data;

    HandlingRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for HandlingRandomizer.
    static HandlingRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();
};
