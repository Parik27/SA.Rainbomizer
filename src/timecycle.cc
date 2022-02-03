#include "timecycle.hh"
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

static int  lastForcedWeather;
static int  lastNewWeather = 0;

static bool s_bEnableRandomTcyc = true;

struct TimecycleSample
{

    int   Amb_R;
    int   Amb_G;
    int   Amb_B;
    int   Amb_Obj_R;
    int   Amb_Obj_G;
    int   Amb_Obj_B;
    int   Dir_R;
    int   Dir_G;
    int   Dir_B;
    int   SkyTop_R;
    int   SkyTop_G;
    int   SkyTop_B;
    int   SkyBot_R;
    int   SkyBot_G;
    int   SkyBot_B;
    int   SunCore_R;
    int   SunCore_G;
    int   SunCore_B;
    int   SunCorona_R;
    int   SunCorona_G;
    int   SunCorona_B;
    float SunSz;
    float SprSz;
    float SprBght;
    int   Shdw;
    int   LightShd;
    int   PoleShd;
    float FarClp;
    float FogSt;
    float LightOnGround;
    int   LowCloudsR;
    int   LowCloudsG;
    int   LowCloudsB;
    int   BottomCloudR;
    int   BottomCloudG;
    int   BottomCloudB;
    float WaterR;
    float WaterG;
    float WaterB;
    float WaterA;
    float Alpha1;
    float RGB1R;
    float RGB1G;
    float RGB1B;
    float Alpha2;
    float RGB2R;
    float RGB2G;
    float RGB2B;
    float CloudAlpha1;
    int   CloudAlpha2;
    int   CloudAlpha3;
    float Illumination;

    void
    Extract (int *Amb_R, int *Amb_G, int *Amb_B, int *Amb_Obj_R, int *Amb_Obj_G,
             int *Amb_Obj_B, int *Dir_R, int *Dir_G, int *Dir_B, int *SkyTop_R,
             int *SkyTop_G, int *SkyTop_B, int *SkyBot_R, int *SkyBot_G,
             int *SkyBot_B, int *SunCore_R, int *SunCore_G, int *SunCore_B,
             int *SunCorona_R, int *SunCorona_G, int *SunCorona_B, float *SunSz,
             float *SprSz, float *SprBght, int *Shdw, int *LightShd,
             int *PoleShd, float *FarClp, float *FogSt, float *LightOnGround,
             int *LowCloudsR, int *LowCloudsG, int *LowCloudsB,
             int *BottomCloudR, int *BottomCloudG, int *BottomCloudB,
             float *WaterR, float *WaterG, float *WaterB, float *WaterA,
             float *Alpha1, float *RGB1R, float *RGB1G, float *RGB1B,
             float *Alpha2, float *RGB2R, float *RGB2G, float *RGB2B,
             float *CloudAlpha1, int *CloudAlpha2, int *CloudAlpha3,
             float *Illumination)
    {
        *Amb_R         = this->Amb_R;
        *Amb_G         = this->Amb_G;
        *Amb_B         = this->Amb_B;
        *Amb_Obj_R     = this->Amb_Obj_R;
        *Amb_Obj_G     = this->Amb_Obj_G;
        *Amb_Obj_B     = this->Amb_Obj_B;
        *Dir_R         = this->Dir_R;
        *Dir_G         = this->Dir_G;
        *Dir_B         = this->Dir_B;
        *SkyTop_R      = this->SkyTop_R;
        *SkyTop_G      = this->SkyTop_G;
        *SkyTop_B      = this->SkyTop_B;
        *SkyBot_R      = this->SkyBot_R;
        *SkyBot_G      = this->SkyBot_G;
        *SkyBot_B      = this->SkyBot_B;
        *SunCore_R     = this->SunCore_R;
        *SunCore_G     = this->SunCore_G;
        *SunCore_B     = this->SunCore_B;
        *SunCorona_R   = this->SunCorona_R;
        *SunCorona_G   = this->SunCorona_G;
        *SunCorona_B   = this->SunCorona_B;
        *SunSz         = this->SunSz;
        *SprSz         = this->SprSz;
        *SprBght       = this->SprBght;
        *Shdw          = this->Shdw;
        *LightShd      = this->LightShd;
        *PoleShd       = this->PoleShd;
        *FarClp        = this->FarClp;
        *FogSt         = this->FogSt;
        *LightOnGround = this->LightOnGround;
        *LowCloudsR    = this->LowCloudsR;
        *LowCloudsG    = this->LowCloudsG;
        *LowCloudsB    = this->LowCloudsB;
        *BottomCloudR  = this->BottomCloudR;
        *BottomCloudG  = this->BottomCloudG;
        *BottomCloudB  = this->BottomCloudB;
        *WaterR        = this->WaterR;
        *WaterG        = this->WaterG;
        *WaterB        = this->WaterB;
        *WaterA        = this->WaterA;
        *Alpha1        = this->Alpha1;
        *RGB1R         = this->RGB1R;
        *RGB1G         = this->RGB1G;
        *RGB1B         = this->RGB1B;
        *Alpha2        = this->Alpha2;
        *RGB2R         = this->RGB2R;
        *RGB2G         = this->RGB2G;
        *RGB2B         = this->RGB2B;
        *CloudAlpha1   = this->CloudAlpha1;
        *CloudAlpha2   = this->CloudAlpha2;
        *CloudAlpha3   = this->CloudAlpha3;
        *Illumination  = this->Illumination;
    }

    void
    Randomize ()
    {
        Amb_R         = random (100);
        Amb_G         = random (100);
        Amb_B         = random (100);
        //Amb_Obj_R     = random (50, 255);
        //Amb_Obj_G     = random (50, 255);
        //Amb_Obj_B     = random (50, 255);
        Dir_R         = 255;
        Dir_G         = 255;
        Dir_B         = 255;
        SkyTop_R      = random (255);
        SkyTop_G      = random (255);
        SkyTop_B      = random (255);
        SkyBot_R      = random (255);
        SkyBot_G      = random (255);
        SkyBot_B      = random (255);
        SunCore_R     = random (255);
        SunCore_G     = random (255);
        SunCore_B     = random (255);
        SunCorona_R   = random (255);
        SunCorona_G   = random (255);
        SunCorona_B   = random (255);
        SunSz         = randomFloat (0.0f, 0.1f);
        SprSz         = randomFloat (0.0f, 0.1f);
        SprBght       = randomFloat (0.0f, 0.1f);
        Shdw          = random (255);
        LightShd      = random (255);
        PoleShd       = random (255);
        FarClp        = random (100, 1500);
        FogSt         = random (-22, 100);
        LightOnGround = randomFloat (0.0f, 0.1f);
        LowCloudsR    = random (255);
        LowCloudsG    = random (255);
        LowCloudsB    = random (255);
        BottomCloudR  = random (255);
        BottomCloudG  = random (255);
        BottomCloudB  = random (255);
        WaterR        = random (255);
        WaterG        = random (255);
        WaterB        = random (255);
        WaterA        = random (30, 255);
        Alpha1        = random (255);
        RGB1R         = random (100);
        RGB1G         = random (100);
        RGB1B         = random (100);
        Alpha2        = random (255);
        RGB2R         = random (100);
        RGB2G         = random (100);
        RGB2B         = random (100);
        CloudAlpha1   = random (255);
        CloudAlpha2   = random (255);
        CloudAlpha3   = random (255);
        Illumination  = 0;
    }
};

/*******************************************************/
void
ChangeForceWeather (short weather)
{
    if (weather == lastForcedWeather)
        {
            weather = lastNewWeather;
        }
    else
        {
            lastForcedWeather = weather;
            lastNewWeather    = random (19);
            weather           = lastNewWeather;
        }
    injector::WriteMemory (0xC81318, weather); // ForcedWeatherType
    injector::WriteMemory (0xC81320, weather); // OldWeatherType
    injector::WriteMemory (0xC8131C, weather); // NewWeatherType
}

/*******************************************************/
void
ChangeTimeCycleValues (
    char *input, char *format, int *Amb_R, int *Amb_G, int *Amb_B,
    int *Amb_Obj_R, int *Amb_Obj_G, int *Amb_Obj_B, int *Dir_R, int *Dir_G,
    int *Dir_B, int *SkyTop_R, int *SkyTop_G, int *SkyTop_B, int *SkyBot_R,
    int *SkyBot_G, int *SkyBot_B, int *SunCore_R, int *SunCore_G,
    int *SunCore_B, int *SunCorona_R, int *SunCorona_G, int *SunCorona_B,
    float *SunSz, float *SprSz, float *SprBght, int *Shdw, int *LightShd,
    int *PoleShd, float *FarClp, float *FogSt, float *LightOnGround,
    int *LowCloudsR, int *LowCloudsG, int *LowCloudsB, int *BottomCloudR,
    int *BottomCloudG, int *BottomCloudB, float *WaterR, float *WaterG,
    float *WaterB, float *WaterA, float *Alpha1, float *RGB1R, float *RGB1G,
    float *RGB1B, float *Alpha2, float *RGB2R, float *RGB2G, float *RGB2B,
    float *CloudAlpha1, int *CloudAlpha2, int *CloudAlpha3, float *Illumination)
{
    static std::vector<TimecycleSample> s_Timesamples;

    if (s_Timesamples.size () != 23 * 8)
        {
            auto &sample = s_Timesamples.emplace_back ();

            sscanf (input, format, &sample.Amb_R, &sample.Amb_G, &sample.Amb_B,
                    &sample.Amb_Obj_R, &sample.Amb_Obj_G, &sample.Amb_Obj_B,
                    &sample.Dir_R, &sample.Dir_G, &sample.Dir_B,
                    &sample.SkyTop_R, &sample.SkyTop_G, &sample.SkyTop_B,
                    &sample.SkyBot_R, &sample.SkyBot_G, &sample.SkyBot_B,
                    &sample.SunCore_R, &sample.SunCore_G, &sample.SunCore_B,
                    &sample.SunCorona_R, &sample.SunCorona_G,
                    &sample.SunCorona_B, &sample.SunSz, &sample.SprSz,
                    &sample.SprBght, &sample.Shdw, &sample.LightShd,
                    &sample.PoleShd, &sample.FarClp, &sample.FogSt,
                    &sample.LightOnGround, &sample.LowCloudsR,
                    &sample.LowCloudsG, &sample.LowCloudsB,
                    &sample.BottomCloudR, &sample.BottomCloudG,
                    &sample.BottomCloudB, &sample.WaterR, &sample.WaterG,
                    &sample.WaterB, &sample.WaterA, &sample.Alpha1,
                    &sample.RGB1R, &sample.RGB1G, &sample.RGB1B, &sample.Alpha2,
                    &sample.RGB2R, &sample.RGB2G, &sample.RGB2B,
                    &sample.CloudAlpha1, &sample.CloudAlpha2,
                    &sample.CloudAlpha3, &sample.Illumination);
        }

    TimecycleSample smpl = GetRandomElement (s_Timesamples);
    if (s_bEnableRandomTcyc)
        smpl.Randomize ();

    smpl.Extract (Amb_R, Amb_G, Amb_B, Amb_Obj_R, Amb_Obj_G, Amb_Obj_B, Dir_R,
                  Dir_G, Dir_B, SkyTop_R, SkyTop_G, SkyTop_B, SkyBot_R,
                  SkyBot_G, SkyBot_B, SunCore_R, SunCore_G, SunCore_B,
                  SunCorona_R, SunCorona_G, SunCorona_B, SunSz, SprSz, SprBght,
                  Shdw, LightShd, PoleShd, FarClp, FogSt, LightOnGround,
                  LowCloudsR, LowCloudsG, LowCloudsB, BottomCloudR,
                  BottomCloudG, BottomCloudB, WaterR, WaterG, WaterB, WaterA,
                  Alpha1, RGB1R, RGB1G, RGB1B, Alpha2, RGB2R, RGB2G, RGB2B,
                  CloudAlpha1, CloudAlpha2, CloudAlpha3, Illumination);
}

/*******************************************************/
void
TimeCycleRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig (
            "TimeCycleRandomizer",
            std::pair ("RandomizeTimeCycle", &m_Config.RandomizeTimeCycle),
            std::pair ("RandomTimecycleOdds", &m_Config.RandomTimecycleOdds),
            std::pair ("ChangeOnFade", &m_Config.ChangeOnFade),
            std::pair ("RandomizeWeather", &m_Config.RandomizeWeather)))
        return;

    if (m_Config.RandomizeWeather)
        {
            RegisterHooks (
                {{HOOK_JUMP, 0x72A4F0, (void *) &ChangeForceWeather}});
            FadesManager::AddFadeCallback (Call<0x72A4F0>);
            FadesManager::AddFadeCallback (
                [] { lastNewWeather = random (19); });
        }

    if (m_Config.RandomizeTimeCycle)
        {
            RegisterHooks (
                {{HOOK_CALL, 0x5BBCE2, (void *) &ChangeTimeCycleValues}});

            if (m_Config.ChangeOnFade)
                FadesManager::AddFadeCallback ([] () {
                    s_bEnableRandomTcyc
                        = random (100) < m_Config.RandomTimecycleOdds;
                    Call<0x5BBAC0> ();
                });
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
