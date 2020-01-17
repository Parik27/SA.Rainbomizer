#include "missions.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "functions.hh"
#include <vector>
#include "injector/injector.hpp"
#include "config.hh"
#include "scrpt.hh"
#include <algorithm>
#include "injector/calling.hpp"
#include "autosave.hh"
#include "missions_data.hh"
#include <stdexcept>
#include <array>

MissionRandomizer *MissionRandomizer::mInstance = nullptr;

const int START_MISSIONS     = 11;
const int END_MISSIONS       = 112;
const int UNLOCKED_CITY_STAT = 181;

/*******************************************************/
void __fastcall RandomizeMissionToStart (CRunningScript *scr, void *edx,
                                         short count)
{
    auto missionRandomizer = MissionRandomizer::GetInstance ();

    scr->CollectParameters (count);
    if (ScriptParams[0] >= START_MISSIONS && ScriptParams[0] <= END_MISSIONS)
        {
            missionRandomizer->mOriginalMissionNumber = ScriptParams[0];

            ScriptParams[0]
                = missionRandomizer->GetRandomMission (ScriptParams[0]);

            missionRandomizer->mRandomizedMissionNumber = ScriptParams[0];

            missionRandomizer->mStoreNextMission = true;
        }
}

/*******************************************************/
void
MissionRandomizer::TeleportPlayerAfterMission ()
{
    try
        {
            Position pos = missionEndPos.at (mOriginalMissionNumber)[0];
            pos.z        = (mRandomizedMissionNumber == 80) ? 100 : pos.z;
            CRunningScript::SetCharCoordinates (FindPlayerPed (),
                                                {pos.x, pos.y, pos.z}, 1, 1);
            FindPlayerEntity ()->SetHeading (pos.heading * 3.1415926 / 180.0);
        }
    catch (const std::out_of_range &e)
        {
        }
}

/*******************************************************/
int
MissionRandomizer::GetRandomMission (int originalMission)
{
    auto exceptions = {
        40, // First Date
        35, // Race Tournament / 8-track / Dirt Track
        83, // Learning to Fly
        71, // Back To School
    };

    for (auto i : exceptions)
        if (originalMission == i)
            return originalMission;

    // Forced Mission
    auto config = ConfigManager::GetInstance ()->GetConfigs ().missions;
    if (config.forcedMissionEnabled)
        return config.forcedMissionID;

    const int MAX_RETRIES = 20;
    for (int i = 0; i < MAX_RETRIES; i++)
        {
            int randomMission = random (START_MISSIONS, END_MISSIONS);
            if (std::find (std::begin (exceptions), std::end (exceptions),
                           randomMission)
                == std::end (exceptions))
                return randomMission;
        }

    return originalMission;
}

/*******************************************************/
bool
MissionRandomizer::ShouldJump (CRunningScript *scr)
{
    const int OPCODE_END_THREAD = 78;
    const int OPCODE_RETURN     = 81;

    int currentOffset = scr->m_pCurrentIP - scr->m_pBaseIP;
    if (currentOffset != this->mPrevOffset)
        {
            short opCode = *reinterpret_cast<uint16_t *> (scr->m_pCurrentIP);
            if (opCode == OPCODE_RETURN && mScriptReplaced)
                {
                    // Restore original base ip
                    scr->m_pBaseIP = mOriginalBaseIP;

                    // Restore local variables
                    memcpy ((int *) 0xA48960, this->mLocalVariables,
                            0x400 * sizeof (uint32_t));

                    mScriptReplaced = false;

                    RestoreCityInfo (this->mCityInfo);
                    this->mRandomizedScript = nullptr;
                }
            else if (opCode == OPCODE_END_THREAD)
                {
                    RestoreCityInfo (this->mCityInfo);
                    this->mRandomizedScript = nullptr;
                }

            this->mPrevOffset = currentOffset;
        }
    if (scr == this->mRandomizedScript)
        {
            for (auto i : threadFinishes[this->mRandomizedMissionNumber])
                {
                    if (i == currentOffset)
                        return true;
                }
        }
    return false;
}

/*******************************************************/
void __fastcall UpdatePCHook (CRunningScript *scr, void *edx, int a2)
{
    CallMethod<0x464DA0> (scr, a2);
}

/*******************************************************/
void
MissionRandomizer::ApplyMissionSpecificFixes (uint8_t *data)
{
    printf ("%d\n", this->mOriginalMissionNumber);
    switch (this->mOriginalMissionNumber)
        {

        // CESAR1
        case 36:
            data += 19711;
            data = Scrpt::CreateOpcode (0x8, "incrmt_var", data,
                                        GlobalVar (457), 1);
            data = Scrpt::CreateOpcode (0x30C, "player_made_progress", data, 1);
            data = Scrpt::CreateOpcode (0x318, "set_latest_mission_passed",
                                        data, "CESAR_1");
            data = Scrpt::CreateOpcode (0x51, "return", data);
            break;

        // Green Sabre
        case 38:
            data += 22051;
            data = Scrpt::CreateOpcode (0x2, "jmp", data, -22257);
            break;

        // Wu Zi Mu / Farewell My Love
        case 48:
            if (mRandomizedMissionNumber != 37)
                ScriptSpace[492] += 5;

            AutoSave::GetInstance ()->SetShouldSave (true);

        // Doberman
        case 21:
            data += 6778;
            data = Scrpt::CreateOpcode (0x879, "enable_gang_wars", data, 1);
            data = Scrpt::CreateOpcode (0x51, "return", data);
            break;

            // Customs Fast Track
        case 69:
            // STEAL4_25110
            data += 30814;
            data = Scrpt::CreateOpcode (0x2, "jmp", data, -25110);

            break;
        }
}

/*******************************************************/
int
MissionRandomizer::GetCorrectedMissionNo ()
{
    return this->mOriginalMissionNumber == 36 ? 35
                                              : this->mOriginalMissionNumber;
}

/*******************************************************/
void
MissionRandomizer::MoveScriptToOriginalOffset (CRunningScript *scr)
{
    int *missionOffsets = (int *) 0xA444C8;

    int baseOffset = missionOffsets[this->GetCorrectedMissionNo ()];
    int offset     = threadFinishes[this->GetCorrectedMissionNo ()][0];

    FILE *scm = fopen (GetGameDirRelativePathA ("data/script/main.scm"), "rb");
    fseek (scm, baseOffset, SEEK_SET);

    mScriptReplaced = true;
    mOriginalBaseIP = scr->m_pBaseIP;

    fread (this->mTempMissionData, 1, 69000, scm);
    scr->m_pBaseIP    = this->mTempMissionData;
    scr->m_pCurrentIP = scr->m_pBaseIP + offset;

    this->ApplyMissionSpecificFixes (this->mTempMissionData);
    this->TeleportPlayerAfterMission ();

    memcpy (this->mLocalVariables, (int *) 0xA48960, 0x400 * sizeof (uint32_t));
    memset ((int *) 0xA48960, 0, 0x400 * sizeof (uint32_t));

    fclose (scm);
}

/*******************************************************/
void
JumpOnMissionEnd ()
{
    static int addr = HookManager::GetOriginalCall ((void *) &JumpOnMissionEnd);
    auto       missionRandomizer = MissionRandomizer::GetInstance ();

    if (missionRandomizer->mRandomizedScript
        && missionRandomizer->ShouldJump (missionRandomizer->mRandomizedScript))
        missionRandomizer->MoveScriptToOriginalOffset (
            missionRandomizer->mRandomizedScript);

    if (addr)
        injector::cstd<void ()>::call (addr);
    else
        (*((int *) 0xA447F4))++;
}

/*******************************************************/
void
MissionRandomizer::ApplyMissionStartSpecificFixes (unsigned char *data)
{
}

/*******************************************************/
bool
IsIPLEnabled (char *name)
{
    int     index = CIplStore::FindIplSlot (name);
    IplDef *def   = CIplStore::ms_pPool->GetAt<IplDef> (index);
    if (!def->field2D || !def->m_bDisableDynamicStreaming)
        return false;

    return true;
}

/*******************************************************/
void
MissionRandomizer::StoreCityInfo ()
{
    this->mCityInfo.citiesUnlocked = CStats::GetStatValue (UNLOCKED_CITY_STAT);
    this->mCityInfo.LVBarriers     = IsIPLEnabled ((char *) "BARRIERS2");
    this->mCityInfo.SFBarriers     = IsIPLEnabled ((char *) "BARRIERS1");
}

/*******************************************************/
void
MissionRandomizer::RestoreCityInfo (const CitiesInfo &info)
{
    printf ("%d -> %d", info.citiesUnlocked, info.citiesUnlocked);
    Scrpt::CallOpcode (0x629, "change_int_stat", UNLOCKED_CITY_STAT,
                       info.citiesUnlocked);

    static auto handleBridge = [] (bool bridge, const char *id) {
        if (bridge)
            Scrpt::CallOpcode (0x776, "create_objects", id);
        else
            Scrpt::CallOpcode (0x777, "remove_objects", id);
    };

    handleBridge (info.LVBarriers, "BARRIERS2");
    handleBridge (info.SFBarriers, "BARRIERS1");
}

/*******************************************************/
void
MissionRandomizer::UnlockCitiesBasedOnMissionID (int missionId)
{
    static std::array<std::pair<int, CitiesInfo>, 4> cities
        = {{{92, {3, false, false}},
            {63, {2, false, false}},
            {38, {1, false, true}},
            {0, {0, true, true}}}};

    if (!this->mRandomizedScript)
        StoreCityInfo ();

    for (auto i : cities)
        {
            if (i.first < missionId)
                {
                    RestoreCityInfo (i.second);
                    return;
                }
        }
}

/*******************************************************/
CRunningScript *
StoreRandomizedScript (uint8_t *startIp)
{
    auto missionRandomizer = MissionRandomizer::GetInstance ();

    auto out = injector::cstd<CRunningScript *(uint8_t *)>::call (
        HookManager::GetOriginalCall ((void *) &StoreRandomizedScript),
        startIp);

    if (missionRandomizer->mStoreNextMission)
        {
            missionRandomizer->ApplyMissionStartSpecificFixes (startIp);
            missionRandomizer->UnlockCitiesBasedOnMissionID (
                missionRandomizer->mRandomizedMissionNumber);

            missionRandomizer->mRandomizedScript = out;
        }

    missionRandomizer->mStoreNextMission = false;
    return out;
}

/*******************************************************/
void
MissionRandomizer::Initialise ()
{

    auto config = ConfigManager::GetInstance ()->GetConfigs ().missions;

    if (!config.enabled)
        return;

    if (!mTempMissionData)
        mTempMissionData = new unsigned char[69000];
    if (!mLocalVariables)
        mLocalVariables = new int[1024];

    RegisterHooks ({{HOOK_CALL, 0x489929, (void *) &RandomizeMissionToStart},
                    {HOOK_CALL, 0x489A7A, (void *) &StoreRandomizedScript}});

    RegisterDelayedHooks ({{HOOK_CALL, 0x469FB0, (void *) &JumpOnMissionEnd}});
    RegisterDelayedFunction ([] { injector::MakeNOP (0x469fb5, 2); });

    Logger::GetLogger ()->LogMessage ("Intialised MissionRandomizer");
}

/*******************************************************/
void
MissionRandomizer::DestroyInstance ()
{
    if (MissionRandomizer::mInstance)
        delete MissionRandomizer::mInstance;
}

/*******************************************************/
MissionRandomizer *
MissionRandomizer::GetInstance ()
{
    if (!MissionRandomizer::mInstance)
        {
            MissionRandomizer::mInstance = new MissionRandomizer ();
            atexit (&MissionRandomizer::DestroyInstance);
        }
    return MissionRandomizer::mInstance;
}
