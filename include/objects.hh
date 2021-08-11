#pragma once

class ObjectsRandomizer
{
    static ObjectsRandomizer *mInstance;

    ObjectsRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for ObjectsRandomizer.
    static ObjectsRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();

    //static inline int increaseObject = 163;
};
