#pragma once

#include <vector>

struct CAnimationStyleDescriptor;

class AnimationRandomizer
{
    std::vector<CAnimationStyleDescriptor *> mAnimAssocDefs;
    static AnimationRandomizer *             mInstance;

    static CAnimationStyleDescriptor *AddAnimAssocDefinition (const char *,
                                                              const char *, int,
                                                              unsigned int,
                                                              void *);

    static void RandomizeAnimAssocDefs ();

    AnimationRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for AnimationRandomizer.
    static AnimationRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();
};
