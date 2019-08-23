#pragma once

struct cHandlingDataMgr;
int __fastcall RandomizeHandling (cHandlingDataMgr *handling, void *edx);

class HandlingRandomizer
{
    static HandlingRandomizer *mInstance;

    HandlingRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for HandlingRandomizer.
    static HandlingRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();
};
