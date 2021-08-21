#include "particles.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "functions.hh"
#include "injector/calling.hpp"
#include "config.hh"

ParticleRandomizer *ParticleRandomizer::mInstance = nullptr;

/*******************************************************/
FxSystemBP_c *__fastcall RandomizeParticles (FxManager_c *thisManager,
                                             void *edx, char *name)
{
    int           particle = random (thisManager->SystemBlueprints.count - 1);
    FxSystemBP_c *randomParticle = thisManager->SystemBlueprints.last;
    FxSystemBP_c *originalParticle
        = injector::thiscall<FxSystemBP_c *(FxManager_c *, char *)>::call (
            0x4A9360, thisManager, name);

    std::string particle_name = name;
    if (particle_name == "heli_dust" || particle_name == "prt_cardebris"
        || particle_name == "gunsmoke")
        return originalParticle;

    while (particle--)
        randomParticle = randomParticle->m_link.prev;

    return randomParticle;
}

/*******************************************************/
void
ParticleRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("ParticleRandomizer"))
        return;

    RegisterHooks ({{HOOK_CALL, 0x4A9BF9, (void *) RandomizeParticles}});
    Logger::GetLogger ()->LogMessage ("Intialised ParticleRandomizer");
}

/*******************************************************/
void
ParticleRandomizer::DestroyInstance ()
{
    if (ParticleRandomizer::mInstance)
        delete ParticleRandomizer::mInstance;
}

/*******************************************************/
ParticleRandomizer *
ParticleRandomizer::GetInstance ()
{
    if (!ParticleRandomizer::mInstance)
        {
            ParticleRandomizer::mInstance = new ParticleRandomizer ();
            atexit (&ParticleRandomizer::DestroyInstance);
        }
    return ParticleRandomizer::mInstance;
}
