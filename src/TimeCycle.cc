#include "TimeCycle.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "functions.hh"
#include "injector/calling.hpp"
#include "config.hh"
#include <chrono>
#include "fades.hh"
#include <algorithm>
#include <vector>
#include <random>
#include <time.h>

TimeCycleRandomizer *TimeCycleRandomizer::mInstance = nullptr;

static std::vector<int> weatherRegions = {0, 1, 2, 3, 4};

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
void
ShuffleWeatherRegions (CVector *coors)
{
    if (!coors)
        coors = &FindPlayerCoors ();
    if (coors->x <= 1000.0f || coors->y <= 910.0f)
    {
        if (coors->x <= -850.0f || coors->x >= 1000.0f || coors->y <= 1280.0f)
        {
            if (coors->x >= -1430.0f || coors->y <= -580.0f || coors->y >= 1430.0f)
            {
                if (coors->x <= 250.0f || coors->x >= 3000.0f || coors->y <= -3000.0f || coors->y >= -850.0f)
                {
                    injector::WriteMemory<short> (0xC81314, weatherRegions[0]);
                }
                else
                {
                    injector::WriteMemory<short> (0xC81314, weatherRegions[1]);
                }
            }
            else
            {
                injector::WriteMemory<short> (0xC81314, weatherRegions[2]);
            }
        }
        else
        {
            injector::WriteMemory<short> (0xC81314, weatherRegions[4]);
        }
    }
    else
    {
        injector::WriteMemory<short> (0xC81314, weatherRegions[3]);
    }
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
    *Amb_R = random (100);
    *Amb_G         = random (100);
    *Amb_B         = random (100);
    *Amb_Obj_R     = random (50, 255);
    *Amb_Obj_G     = random (50, 255);
    *Amb_Obj_B     = random (50, 255);
    *Dir_R         = 255;
    *Dir_G         = 255;
    *Dir_B         = 255;
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
    *SunSz         = randomFloat (0.0f, 0.1f);
    *SprSz = randomFloat (0.0f, 0.1f);
    *SprBght       = randomFloat (0.0f, 0.1f);
    *Shdw          = random (255);
    *LightShd      = random (255);
    *PoleShd       = random (255);
    *FarClp        = random (100, 1500);
    *FogSt         = random (-22, 100);
    *LightOnGround = randomFloat (0.0f, 0.1f);
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
void __fastcall LogTimeCycleValues (CTimeCycleCurrent *timecyc, void *edx, int weatherID,
                            int timeID)
{
    CallMethod<0x55F4B0> (timecyc, weatherID, timeID);
    Logger::GetLogger()->LogMessage("Amb RGB: " + std::to_string(timecyc->m_fAmbientRed) + ", " + std::to_string(timecyc->m_fAmbientGreen) + ", " + std::to_string(timecyc->m_fAmbientBlue));
    Logger::GetLogger ()->LogMessage (
        "Amb Obj RGB: " + std::to_string (timecyc->m_fAmbientRed_Obj) + ", "
        + std::to_string (timecyc->m_fAmbientGreen_Obj) + ", "
        + std::to_string (timecyc->m_fAmbientBlue_Obj));
    Logger::GetLogger ()->LogMessage (
        "Sky Top RGB: " + std::to_string (timecyc->m_wSkyTopRed) + ", "
        + std::to_string (timecyc->m_wSkyTopGreen) + ", "
        + std::to_string (timecyc->m_wSkyTopBlue));
    Logger::GetLogger ()->LogMessage (
        "Sky Bottom RGB: " + std::to_string (timecyc->m_wSkyBottomRed) + ", "
        + std::to_string (timecyc->m_wSkyBottomGreen) + ", "
        + std::to_string (timecyc->m_wSkyBottomBlue));
    Logger::GetLogger ()->LogMessage (
        "Sun Core RGB: " + std::to_string (timecyc->m_wSunCoreRed) + ", "
        + std::to_string (timecyc->m_wSunCoreGreen) + ", "
        + std::to_string (timecyc->m_wSunCoreBlue));
    Logger::GetLogger ()->LogMessage (
        "Sun Corona RGB: " + std::to_string (timecyc->m_wSunCoronaRed) + ", "
        + std::to_string (timecyc->m_wSunCoronaGreen) + ", "
        + std::to_string (timecyc->m_wSunCoronaBlue));
    Logger::GetLogger ()->LogMessage (
        "Sun Size: " + std::to_string (timecyc->m_fSunSize));
    Logger::GetLogger ()->LogMessage ("Sprite Size: "
                                      + std::to_string (timecyc->m_fSpriteSize));
    Logger::GetLogger ()->LogMessage (
        "Sprite Brightness: " + std::to_string (timecyc->m_fSpriteBrightness));
    Logger::GetLogger ()->LogMessage (
        "Shadows (base, light, pole): "
        + std::to_string (timecyc->m_wShadowStrength) + ", "
        + std::to_string (timecyc->m_wLightShadowStrength) + ", "
        + std::to_string (timecyc->m_wPoleShadowStrength));
    Logger::GetLogger ()->LogMessage ("Far Clip: "
                                      + std::to_string (timecyc->m_fFarClip));
    Logger::GetLogger ()->LogMessage ("Fog Strength: "
                                      + std::to_string (timecyc->m_fFogSt));
    Logger::GetLogger ()->LogMessage ("Light On Ground: "
                                      + std::to_string (timecyc->m_fLightOnGround));
    Logger::GetLogger ()->LogMessage (
        "Low Clouds RGB: " + std::to_string (timecyc->m_wLowCloudsRed) + ", "
        + std::to_string (timecyc->m_wLowCloudsGreen) + ", "
        + std::to_string (timecyc->m_wLowCloudsBlue));
    Logger::GetLogger ()->LogMessage (
        "Bottom Clouds RGB: " + std::to_string (timecyc->m_wBottomCloudsRed) + ", " + std::to_string (timecyc->m_wBottomCloudsGreen) + ", "
        + std::to_string (timecyc->m_wBottomCloudsBlue));
    Logger::GetLogger ()->LogMessage (
        "Water RGBA: " + std::to_string (timecyc->m_fWaterRed) + ", "
        + std::to_string (timecyc->m_fWaterGreen) + ", "
        + std::to_string (timecyc->m_fWaterBlue) + ", "
        + std::to_string (timecyc->m_fWaterAlpha));
    Logger::GetLogger ()->LogMessage (
        "RGBA1: " + std::to_string (timecyc->m_fRGB1_R) + ", "
        + std::to_string (timecyc->m_fRGB1_G) + ", "
        + std::to_string (timecyc->m_fRGB1_B) + ", "
        + std::to_string (timecyc->m_fAlpha1));
    Logger::GetLogger ()->LogMessage (
        "RGBA2: " + std::to_string (timecyc->m_fRGB2_R) + ", "
        + std::to_string (timecyc->m_fRGB2_G) + ", "
        + std::to_string (timecyc->m_fRGB2_B) + ", "
        + std::to_string (timecyc->m_fAlpha2));
    Logger::GetLogger ()->LogMessage (
        "Cloud Alphas 1, 2, 3: " + std::to_string (timecyc->m_fCloudAlpha1) + ", "
        + std::to_string (timecyc->m_dwCloudAlpha2) + ", "
        + std::to_string (timecyc->m_wCloudAlpha3));
    Logger::GetLogger ()->LogMessage (
        "Illumination: " + std::to_string (timecyc->m_fIllumination) + "\n");
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
                {{HOOK_JUMP, 0x72A4F0, (void *) &ChangeForceWeather},
            {HOOK_JUMP, 0x72A640, (void *) &ShuffleWeatherRegions}});
        FadesManager::AddFadeCallback (Call<0x72A4F0>);
        
        FadesManager::AddFadeCallback ([] {
            std::shuffle (weatherRegions.begin (),
                          weatherRegions.end (), std::default_random_engine(time(NULL)));});
    }

    if (m_Config.RandomizeTimeCycle)
    {
        RegisterHooks ({{HOOK_CALL, 0x5BBCE2, (void *) &ChangeTimeCycleValues}/*,
                        {HOOK_CALL, 0x560613, (void *) &LogTimeCycleValues}*/});
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
