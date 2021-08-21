#include "respawns.hh"
#include <cstdlib>
#include "logger.hh"
#include "functions.hh"
#include "util/scrpt.hh"
#include "config.hh"

RespawnPointRandomizer *RespawnPointRandomizer::mInstance = nullptr;

/*******************************************************/
void
RandomizeRespawnPoint (float x, float y, float z, RwV3d *a4, float *a5)
{
    a4->x = randomFloat (-3000.0, 3000.0);
    a4->y = randomFloat (-3000.0, 3000.0);

    Scrpt::CallOpcode (0x4E4, "refresh_game_renderer", a4->x, a4->y);
    Scrpt::CallOpcode (0x3CB, "set_render_origin", a4->x, a4->y, 20);
    Scrpt::CallOpcode (0x15f, "set_pos", a4->x, a4->y, 20, 0, 0, 0);

    a4->z = CWorld::FindGroundZedForCoord (a4->x, a4->y);

    *a5 = randomFloat (0, 360);
}

/*******************************************************/
void
RespawnPointRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("RespawnPointRandomizer"))
        return;

    RegisterHooks ({{HOOK_CALL, 0x44331B, (void *) &RandomizeRespawnPoint},
                    {HOOK_CALL, 0x4435C6, (void *) &RandomizeRespawnPoint},
                    {HOOK_CALL, 0x442F70, (void *) &RandomizeRespawnPoint}});
    Logger::GetLogger ()->LogMessage ("Intialised RespawnPointRandomizer");
}

/*******************************************************/
void
RespawnPointRandomizer::DestroyInstance ()
{
    if (RespawnPointRandomizer::mInstance)
        delete RespawnPointRandomizer::mInstance;
}

/*******************************************************/
RespawnPointRandomizer *
RespawnPointRandomizer::GetInstance ()
{
    if (!RespawnPointRandomizer::mInstance)
        {
            RespawnPointRandomizer::mInstance = new RespawnPointRandomizer ();
            atexit (&RespawnPointRandomizer::DestroyInstance);
        }
    return RespawnPointRandomizer::mInstance;
}
