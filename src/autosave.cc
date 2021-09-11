#include "autosave.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "functions.hh"
#include "config.hh"
#include "injector/injector.hpp"
#include "injector/calling.hpp"
#include "injector/hooking.hpp"
#include "colours.hh"
#include <cmath>
#include <ctime>
#include "generalsettings.hh"

AutoSave *AutoSave::mInstance = nullptr;

/*******************************************************/
void __fastcall SaveFixedCoordinates (char *out, void *edx, CPed *ped)
{
    HookManager::CallOriginal<injector::thiscall<void (char *, CPed *)>,
                              0x5D577E> (out, ped);

    if (AutoSave::GetInstance ()->mSaveVehicleCoords && ped == FindPlayerPed ())
        {
            auto translation = ped->m_pVehicle->GetPosition ();
            memcpy (out, &translation->m_vPosn, sizeof (CVector));
            AutoSave::GetInstance ()->mSaveVehicleCoords = false;
        }
}

/*******************************************************/
int __fastcall HandleAutosave (CRunningScript *scr, void *edx)
{
    AutoSave::GetInstance ()->RevertHooks ();
    scr->EndThread ();
    AutoSave::GetInstance ()->InstallHooks ();

    if (scr->m_bIsMission && AutoSave::GetInstance ()->ShouldSave ())
        {
            scr->m_bIsActive = false;

            // Save in a vehicle
            int *playerFlags   = FindPlayerPed (0)->flags;
            int  originalFlags = playerFlags[0];

            if (playerFlags[0] & 0x100)
                AutoSave::GetInstance ()->mSaveVehicleCoords = true;

            playerFlags[0] &= ~0x100;

            // Save
            CGenericGameStorage::MakeValidSaveFileName (
                GeneralSettings::m_Config.AutoSaveSlot - 1);
            CGenericGameStorage::GenericSave ();

            AutoSave::GetInstance ()->SetShouldSave (false);

            playerFlags[0] = originalFlags;
        }
    return 0;
}

/*******************************************************/
void
SetShouldSave (uint16_t id, float val)
{
    AutoSave::GetInstance ()->SetShouldSave (true);
    CStats::IncrementStat (id, val);
}

/*******************************************************/
char *__fastcall SetShouldSaveGlobalVars (CRunningScript *scr, void *edx,
                                          int a2)
{
    char *var        = scr->GetPointerToScriptVariable (a2);
    int   global_var = (var - (char *) ScriptSpace) / 4;

    if (AutoSave::GetInstance ()->IsMissionGlobalVariable (global_var))
        AutoSave::GetInstance ()->SetShouldSave (true);

    return var;
}

/*******************************************************/
void
DrawAutosaveMessage ()
{
    HookManager::CallOriginal<injector::cstd<void ()>, 0x58FCFA> ();
    AutoSave::GetInstance ()->DrawAutosaveMessage ();
}

/*******************************************************/
inline float
CalculateScreenOffsetX (float value, float width = 1600)
{
    return value / width * RsGlobal->MaximumWidth;
}

/*******************************************************/
inline float
CalculateScreenOffsetY (float value, float height = 900)
{
    return value / height * RsGlobal->MaximumHeight;
}

/*******************************************************/
void
AutoSave::DrawMessage (const char *text)
{
    CFont::SetOrientation (ALIGN_LEFT);
    CFont::SetJustify (0);
    CFont::SetFontStyle (1);
    CFont::SetScaleForCurrentlanguage (CalculateScreenOffsetX (1),
                                       CalculateScreenOffsetY (1.5));
    CFont::SetBackground (true, true);
    CFont::SetDropShadowPosition (0);
    CFont::SetWrapx (mDisplayDrawPosX + CalculateScreenOffsetX (500));
    CFont::SetAlphaFade (255);
    CFont::SetBackgroundColor (GetRainbowColour ());
    CFont::SetColor ({00, 0, 0, 255});
    CFont::PrintString (this->mDisplayDrawPosX, this->mDisplayDrawPosY,
                        (char *) text);
}

/*******************************************************/
void
AutoSave::HandleTransitions (float &counter, const float &target)
{
    if (int (target) != int (counter))
        {
            float offset = *ms_fTimeStep;
            offset *= (target - counter) / 10.0f;
            counter += offset;
        }
}

/*******************************************************/
void
AutoSave::DrawAutosaveMessage ()
{
    const int MESSAGE_DURATION = 5;

    DrawMessage ("Autosave Complete");

    if (!mLastSave || time (nullptr) - mLastSave > MESSAGE_DURATION)
        {
            SetDrawXY (CalculateScreenOffsetX (-600), -1);
            mLastSave = 0;
        }
    else
        SetDrawXY (CalculateScreenOffsetX (64), -1);

    HandleTransitions (mDisplayDrawPosX, mDrawPosX);
    HandleTransitions (mDisplayDrawPosY, mDrawPosY);

    SetDrawXY (-1, RsGlobal->MaximumHeight - CalculateScreenOffsetY (64));
}

/*******************************************************/

void
AutoSave::SetShouldSave (bool shouldSave)
{
    if (!shouldSave)
        mLastSave = time (nullptr);

    m_shouldSave = shouldSave;
};

/*******************************************************/
int __fastcall HookDrawRadarRing (void *sprite, void *edx, CRect *rect,
                                  void *color)
{
    float offset = CalculateScreenOffsetY (48);
    AutoSave::GetInstance ()->SetDrawXY (-1, rect->top - offset);

    return HookManager::CallOriginalAndReturn<
        injector::thiscall<int (void *, CRect *, void *)>, 0x58A823> (1, sprite,
                                                                      rect,
                                                                      color);
}

/*******************************************************/
void
HookDrawWideScreenBorders (CRect *rect, CRGBA *colour)
{
    float offset = CalculateScreenOffsetY (48);
    AutoSave::GetInstance ()->SetDrawXY (-1, rect->bottom - offset);

    HookManager::CallOriginal<injector::cstd<void (CRect *, CRGBA *)>,
                              0x514947> (rect, colour);
}

/*******************************************************/
void __stdcall HookDrawVitals (CRect *rect, char *title, char fadeState,
                               CRGBA rgba, int a5, char a6)
{
    float offset = CalculateScreenOffsetY (78);
    AutoSave::GetInstance ()->SetDrawXY (-1, rect->top - offset);

    HookManager::CallOriginal<
        injector::stdcall<void (CRect *, char *, char, CRGBA, int, char)>,
        0x589808> (rect, title, fadeState, rgba, a5, a6);
}

/*******************************************************/
void
AutoSave::Initialise ()
{
    if (GeneralSettings::m_Config.AutoSaveSlot < 1
        || GeneralSettings::m_Config.AutoSaveSlot > 8)
        return;

    this->InstallHooks ();
    RegisterHooks ({{HOOK_CALL, 0x480D11, (void *) &::SetShouldSave},
                    {HOOK_CALL, 0x465FC3, (void *) &::SetShouldSaveGlobalVars},
                    {HOOK_CALL, 0x5D577E, (void *) &SaveFixedCoordinates}});

    RegisterDelayedHooks (
        {{HOOK_CALL, 0x58FCFA, (void *) &::DrawAutosaveMessage},
         {HOOK_CALL, 0x58A823, (void *) &HookDrawRadarRing},
         {HOOK_CALL, 0x514947, (void *) &HookDrawWideScreenBorders},
         {HOOK_CALL, 0x589808, (void *) &HookDrawVitals},
         {HOOK_CALL, 0x5898B2, (void *) &HookDrawVitals}});

    Logger::GetLogger ()->LogMessage ("Intialised AutoSave");
}

/*******************************************************/
bool
AutoSave::IsMissionGlobalVariable (int global_var)
{
    // $629 => $RIOT_TOTAL_PASSED_MISSIONS
    // Note: This is for adding more mission global variables for fine tuning
    //       when saves happen
    for (int variable : {629})
        {
            if (variable == global_var)
                return true;
        }

    return false;
}

/*******************************************************/
void
AutoSave::InstallHooks ()
{
    memcpy (this->m_original_data, (void *) 0x465AA0, ORIGINAL_DATA_SIZE);
    injector::MakeJMP (0x465AA0, (void *) &HandleAutosave);
}

/*******************************************************/
void
AutoSave::RevertHooks ()
{
    memcpy ((void *) 0x465AA0, this->m_original_data, ORIGINAL_DATA_SIZE);
}

/*******************************************************/
void
AutoSave::DestroyInstance ()
{
    if (AutoSave::mInstance)
        delete AutoSave::mInstance;
}

/*******************************************************/
AutoSave *
AutoSave::GetInstance ()
{
    if (!AutoSave::mInstance)
        {
            AutoSave::mInstance = new AutoSave ();
            atexit (&AutoSave::DestroyInstance);
        }
    return AutoSave::mInstance;
}
