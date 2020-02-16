#include "util/loader.hh"
#include "functions.hh"

/*******************************************************/
int
StreamingManager::GetRandomLoadedVehicle ()
{
    return CStreaming::ms_nVehiclesLoaded->PickRandomCar (false, false);
}

/*******************************************************/
eLoadError
StreamingManager::AttemptToLoadVehicle (int model)
{
    eLoadError error = ERR_ALREADY_LOADED;
    const int  TRIES = 1;

    for (int i = 0; ms_aInfoForModel[model].m_nLoadState != 1; i++)
        {
            CStreaming::RequestModel (model, 0);
            CStreaming::LoadAllRequestedModels (false);

            error = ERR_LOADED;
            if (i >= TRIES)
                {
                    return ERR_FAILED;
                }
        }

    return error;
}
