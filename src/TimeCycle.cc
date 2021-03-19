#include "TimeCycle.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "functions.hh"
#include "injector/calling.hpp"
#include "config.hh"
#include <chrono>
#include "fades.hh"

TimeCycleRandomizer *TimeCycleRandomizer::mInstance = nullptr;

/*******************************************************/
void
ChangeForceWeather (short weather)
{
    weather = random (19);
    injector::WriteMemory (0xC81318, weather);
    injector::WriteMemory (0xC81320, weather);
    injector::WriteMemory (0xC8131C, weather);
}

/*******************************************************/
void ChangeTimeCycleValues
(char *input, char *format, int *Amb_R, int *Amb_G, int *Amb_B, int *Amb_Obj_R,
    int *Amb_Obj_G, int *Amb_Obj_B, int *Dir_R, int *Dir_G, int *Dir_B,
    int *SkyTop_R, int *SkyTop_G, int *SkyTop_B, int *SkyBot_R, int *SkyBot_G,
    int *SkyBot_B, int *SunCore_R, int *SunCore_G, int *SunCore_B,
    int *SunCorona_R, int *SunCorona_G, int *SunCorona_B, float *SunSz,
    float *SprSz, float *SprBght, int *Shdw, int *LightShd, int *PoleShd,
    float *FarClp, float *FogSt, float *LightOnGround, int *LowCloudsR,
    int *LowCloudsG, int *LowCloudsB, int *BottomCloudR, int *BottomCloudG,
    int *BottomCloudB, float *WaterR, float *WaterG, float *WaterB,
    float *WaterA, float *Alpha1, float *RGB1R, float *RGB1G, float *RGB1B,
    float *Alpha2, float *RGB2R, float *RGB2G, float *RGB2B, float *CloudAlpha1,
    int *CloudAlpha2, int *CloudAlpha3,
    float *Illumination)
{
    *Amb_R         = random (255);
    *Amb_G         = random (255);
    *Amb_B         = random (255);
    *Amb_Obj_R     = random (255);
    *Amb_Obj_G     = random (255);
    *Amb_Obj_B     = random (255);
    *Dir_R         = random (255);
    *Dir_G         = random (255);
    *Dir_B         = random (255);
    *SkyTop_R      = random (255);
    *SkyTop_G      = random (255);
    *SkyTop_B      = random (255);
    *SkyBot_R      = random (255);
    *SkyBot_G      = random (255);
    *SkyBot_B      = random (255);
    *SunCore_R     = random (255);
    *SunCore_G     = random (255);
    *SunCore_B     = random (255);
    *SunCorona_R   = random (255);
    *SunCorona_G   = random (255);
    *SunCorona_B   = random (255);
    *SunSz         = randomFloat(0.0f, 10.0f);
    *SprSz         = randomFloat (0.0f, 1.0f);
    *SprBght       = randomFloat (0.0f, 1.0f);
    *Shdw          = random (255);
    *LightShd      = random (255);
    *PoleShd       = random (255);
    *FarClp        = random (100, 1500);
    *FogSt         = random (-22, 100);
    *LightOnGround = randomFloat (0.0f, 1.0f);
    *LowCloudsR    = random (255);
    *LowCloudsG    = random (255);
    *LowCloudsB    = random (255);
    *BottomCloudR  = random (255);
    *BottomCloudG  = random (255);
    *BottomCloudB  = random (255);
    *WaterR        = random (255);
    *WaterG        = random (255);
    *WaterB        = random (255);
    *WaterA        = random (255);
    *Alpha1        = random (255);
    *RGB1R         = random (255);
    *RGB1G         = random (255);
    *RGB1B         = random (255);
    *Alpha2        = random (255);
    *RGB2R         = random (255);
    *RGB2G         = random (255);
    *RGB2B         = random (255);
    *CloudAlpha1   = random (255);
    *CloudAlpha2   = random (255);
    *CloudAlpha3   = random (255);
    *Illumination  = 0;
}

/*******************************************************/
void
TimeCycleRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("TimeCycleRandomizer",
        std::pair("RandomizeTimeCycle", &m_Config.RandomizeTimeCycle),
        std::pair("RandomizeWeather", &m_Config.RandomizeWeather)))
        return;

    if (m_Config.RandomizeWeather)
    {
        RegisterHooks (
                {{HOOK_JUMP, 0x72A4F0, (void *) &ChangeForceWeather}});
        FadesManager::AddFadeCallback (Call<0x72A4F0>);
    }

    if (m_Config.RandomizeTimeCycle)
    {
        RegisterHooks ({{HOOK_CALL, 0x5BBCE2, (void *) &ChangeTimeCycleValues}});
            FadesManager::AddFadeCallback (Call<0x5BBAC0>);
    }

    Logger::GetLogger ()->LogMessage ("Intialised TimeCycleRandomizer");
}

/*******************************************************/
void
TimeCycleRandomizer::DestroyInstance ()
{
    if (TimeCycleRandomizer::mInstance)
        delete TimeCycleRandomizer::mInstance;
}

/*******************************************************/
TimeCycleRandomizer *
TimeCycleRandomizer::GetInstance ()
{
    if (!TimeCycleRandomizer::mInstance)
        {
            TimeCycleRandomizer::mInstance = new TimeCycleRandomizer ();
            atexit (&TimeCycleRandomizer::DestroyInstance);
        }
    return TimeCycleRandomizer::mInstance;
}
