#pragma once

class MapRandomizer
{
    static MapRandomizer *mInstance;

    MapRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for MapRandomizer.
    static MapRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();
};
