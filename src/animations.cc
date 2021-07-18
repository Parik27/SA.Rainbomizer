#include "animations.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "functions.hh"
#include "injector/calling.hpp"
#include "fades.hh"
#include "config.hh"

AnimationRandomizer *AnimationRandomizer::mInstance = nullptr;

/*******************************************************/
CAnimBlendAssocGroup *
GetRandomGroup (int anim)
{
    std::vector<int> mValidGroups;
    for (int i = 0; i < CAnimBlendAssocGroup::ms_numAnimAssocDefinitions; i++)
        {
            auto &group = CAnimBlendAssocGroup::ms_aAnimAssocGroups[i];
            if (group.pAnimBlock && group.pAnimBlock->bLoaded
                && group.iNumAnimations >= anim)
                mValidGroups.push_back (i);
        }

    return &CAnimBlendAssocGroup::ms_aAnimAssocGroups[mValidGroups[random (
        mValidGroups.size () - 1)]];
}

/*******************************************************/
CAnimBlendAssociation *__fastcall RandomizeAnimation (
    CAnimBlendAssocGroup *group, void *edx, int animation)
{
    int newAnim = random (group->iNumAnimations - 1) + group->iIDOffset;
    return group->CopyAnimation (newAnim);
}

/*******************************************************/
CAnimationStyleDescriptor *
AnimationRandomizer::AddAnimAssocDefinition (const char *p1, const char *p2,
                                             int p3, unsigned int p4, void *p5)
{
    auto desc = HookManager::CallOriginalAndReturn<
        injector::cstd<CAnimationStyleDescriptor *(const char *, const char *,
                                                   int, unsigned int, void *)>,
        0x5BC99C> ([] () { return nullptr; }, p1, p2, p3, p4, p5);

    GetInstance ()->mAnimAssocDefs.push_back (desc);
    return desc;
}

/*******************************************************/
void
AnimationRandomizer::RandomizeAnimAssocDefs ()
{
    auto &defs = GetInstance ()->mAnimAssocDefs;
    for (auto def : defs)
        {
            for (int i = 0; i < def->animsCount; i++)
                {
                    CAnimationStyleDescriptor *def2
                        = defs[random (defs.size () - 1)];
                    std::swap (def->animNames[i], def2->animNames[i]);
                }
        }
}

/*******************************************************/
void
AnimationRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("AnimationRandomizer"))
        return;

    RegisterHooks ({{HOOK_CALL, 0x5BC99C, (void *) AddAnimAssocDefinition}});
    FadesManager::AddFadeCallback (RandomizeAnimAssocDefs);

    RegisterHooks ({{HOOK_CALL, 0x4D3AB7, (void *) RandomizeAnimation},
                    {HOOK_CALL, 0x4D3A55, (void *) RandomizeAnimation},
                    {HOOK_CALL, 0x4D3B47, (void *) RandomizeAnimation}});
    Logger::GetLogger ()->LogMessage ("Intialised AnimationRandomizer");
}

/*******************************************************/
void
AnimationRandomizer::DestroyInstance ()
{
    if (AnimationRandomizer::mInstance)
        delete AnimationRandomizer::mInstance;
}

/*******************************************************/
AnimationRandomizer *
AnimationRandomizer::GetInstance ()
{
    if (!AnimationRandomizer::mInstance)
        {
            AnimationRandomizer::mInstance = new AnimationRandomizer ();
            atexit (&AnimationRandomizer::DestroyInstance);
        }
    return AnimationRandomizer::mInstance;
}
