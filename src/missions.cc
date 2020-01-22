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
#include <random>

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
            if (missionRandomizer->mSkipMissionNumber != ScriptParams[0])
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
            int status   = GetStatusForTwoPartMissions (mOriginalMissionNumber);
            Position pos = missionEndPos.at (mOriginalMissionNumber)[status];
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
MissionRandomizer::GetStatusForTwoPartMissions (int index)
{
    switch (index)
        {
        // House Party
        case 34: printf ("%d\n", ScriptSpace[455]); return ScriptSpace[455] > 3;

        // Wu Zi Mu and Farewell My Love
        case 48: return ScriptSpace[492] > 5;

        // Jizzy
        case 59: return ScriptSpace[545] > 1;

        default: return 0;
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

    if (config.shufflingEnabled)
        {
            if (mShuffledOrder.count (originalMission))
                {
                    int index = GetStatusForTwoPartMissions (originalMission);
                    if (mShuffledOrder[originalMission].size () <= index)
                        {
                            printf ("The value of originalMission: %d\n",
                                    originalMission);
                            printf ("The size of originalMission: %d\n",
                                    mShuffledOrder[originalMission].size ());
                            index = 0;
                        }

                    return mShuffledOrder[originalMission][index];
                }
        }

    std::vector<uint8_t> missionList;
    for (int i = START_MISSIONS; i < END_MISSIONS; i++)
        {
            if (std::find (std::begin (exceptions), std::end (exceptions), i)
                != std::end (exceptions))
                continue;

            if (mSaveInfo.missionStatus[i])
                missionList.push_back (i);
        }
    printf ("missionList Size: %d\n", missionList.size ());

    if (missionList.size () == 0)
        return originalMission;

    return missionList[random (missionList.size () - 1)];
}

/*******************************************************/
bool
MissionRandomizer::ShouldJump (CRunningScript *scr)
{
    const int OPCODE_END_THREAD           = 78;
    const int OPCODE_RETURN               = 81;
    const int OPCODE_STORE_CAR_CHAR_IS_IN = 0xD9;

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

                    mScriptReplaced         = false;
                    this->mRandomizedScript = nullptr;
                }
            else if (opCode == OPCODE_END_THREAD)
                {
                    RestoreCityInfo (this->mCityInfo);
                    this->mRandomizedScript = nullptr;
                }
            else if (opCode == OPCODE_STORE_CAR_CHAR_IS_IN)
                {
                    // Put player in a random vehicle
                    if (!FindPlayerVehicle ())
                        {
                            Scrpt::CallOpcode (0xa5, "create_car", 567, 0.0f,
                                               0.0f, 0.0f, GlobalVar (2197));
                            Scrpt::CallOpcode (0x036A, "put_actor_in_car",
                                               GlobalVar (3),
                                               ScriptSpace[2197]);
                        }
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

    int baseOffset = missionOffsets[GetCorrectedMissionNo ()];
    int status     = GetStatusForTwoPartMissions (mOriginalMissionNumber);
    int offset     = threadFinishes[GetCorrectedMissionNo ()][status];

    FILE *scm = fopen (GetGameDirRelativePathA ("data/script/main.scm"), "rb");
    fseek (scm, baseOffset, SEEK_SET);

    mScriptReplaced = true;
    mOriginalBaseIP = scr->m_pBaseIP;

    fread (this->mTempMissionData, 1, 69000, scm);
    scr->m_pBaseIP    = this->mTempMissionData;
    scr->m_pCurrentIP = scr->m_pBaseIP + offset;

    this->ApplyMissionSpecificFixes (this->mTempMissionData);
    this->TeleportPlayerAfterMission ();
    RestoreCityInfo (this->mCityInfo);

    mSaveInfo.missionStatus[mRandomizedMissionNumber]--;

    memcpy (this->mLocalVariables, (int *) 0xA48960, 0x400 * sizeof (uint32_t));
    memset ((int *) 0xA48960, 0, 0x400 * sizeof (uint32_t));

    fclose (scm);
}

/*******************************************************/
void
JumpOnMissionEnd ()
{
    auto missionRandomizer = MissionRandomizer::GetInstance ();

    if (missionRandomizer->mRandomizedScript
        && missionRandomizer->ShouldJump (missionRandomizer->mRandomizedScript))
        missionRandomizer->MoveScriptToOriginalOffset (
            missionRandomizer->mRandomizedScript);

    HookManager::CallOriginalAndReturn<injector::cstd<void ()>, 0x469FB0> (
        [] { (*((int *) 0xA447F4))++; });
}

/*******************************************************/
void
MissionRandomizer::ApplyMissionStartSpecificFixes (unsigned char *data)
{
    switch (this->mRandomizedMissionNumber)
        {
        case 36:
            ScriptSpace[457] = 0;
            if (!CRunningScripts::CheckForRunningScript ("cesar"))
                Scrpt::CallOpcode (0x4F, "create_thread", 64462);
            break;
        }
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
    this->mCurrentCitiesUnlocked = info.citiesUnlocked;

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

    auto out = HookManager::CallOriginalAndReturn<
        injector::cstd<CRunningScript *(uint8_t *)>, 0x489A7A> (nullptr,
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
float
UnlockCities (int statsId)
{
    auto missionRandomizer = MissionRandomizer::GetInstance ();
    if (missionRandomizer->mRandomizedScript)
        return missionRandomizer->mCurrentCitiesUnlocked;

    return HookManager::CallOriginalAndReturn<injector::cstd<float (int)>,
                                              0x4417F5> (3, 181);
}

/*******************************************************/
void
SaveMissionData ()
{
    // CTheScripts::Save
    HookManager::CallOriginal<injector::cstd<void ()>, 0x5D15A6> ();
    MissionRandomizer::GetInstance ()->Save ();
}

/*******************************************************/
void
LoadMissionData ()
{
    HookManager::CallOriginal<injector::cstd<void ()>, 0x5D19CE> ();
    MissionRandomizer::GetInstance ()->Load ();
}

/*******************************************************/
void
InitAtNewGame ()
{
    HookManager::CallOriginal<injector::cstd<void ()>, 0x53BE76> ();
    MissionRandomizer::GetInstance ()->ResetSaveData ();
    MissionRandomizer::GetInstance ()->InitShuffledMissionOrder ();
}

/*******************************************************/
void
MissionRandomizer::ResetSaveData ()
{
    auto config = ConfigManager::GetInstance ()->GetConfigs ().missions;
    this->mRandomizedScript = nullptr;

    mSaveInfo.randomSeed = config.shufflingSeed;
    if (config.shufflingEnabled && config.shufflingSeed == -1)
        mSaveInfo.randomSeed = random (INT_MAX);

    for (auto &i : mSaveInfo.missionStatus.data)
        i = 1;

    mSaveInfo.missionStatus[34]++; // House Party
    mSaveInfo.missionStatus[48]++; // Wu Zi Mu and Farewell My Love
    mSaveInfo.missionStatus[59]++; // Jizzy
}

/*******************************************************/
void
MissionRandomizer::Save ()
{
    CGenericGameStorage::SaveDataToWorkBuffer (&mSaveInfo, sizeof (mSaveInfo));
}

/*******************************************************/
void
MissionRandomizer::Load ()
{
    auto config = ConfigManager::GetInstance ()->GetConfigs ().missions;
    MissionRandomizerSaveStructure saveInfo;
    CGenericGameStorage::LoadDataFromWorkBuffer (&saveInfo, sizeof (saveInfo));

    ResetSaveData ();
    if (strncmp (saveInfo.signature, "RAINBOMIZER",
                 sizeof (saveInfo.signature)))
        return InitShuffledMissionOrder ();

    mSaveInfo.randomSeed = saveInfo.randomSeed;
    if (config.forceShufflingSeed && config.shufflingSeed != -1)
        mSaveInfo.randomSeed = config.shufflingSeed;

    InitShuffledMissionOrder ();

    mSaveInfo = saveInfo;
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
                    {HOOK_CALL, 0x489A7A, (void *) &StoreRandomizedScript},
                    {HOOK_CALL, 0x441869, (void *) &UnlockCities},
                    {HOOK_CALL, 0x4417F5, (void *) &UnlockCities},
                    {HOOK_CALL, 0x5D15A6, (void *) &SaveMissionData},
                    {HOOK_CALL, 0x5D19CE, (void *) &LoadMissionData}});

    RegisterDelayedHooks ({{HOOK_CALL, 0x469FB0, (void *) &JumpOnMissionEnd},
                           {HOOK_CALL, 0x53BE76, (void *) &InitAtNewGame}});

    RegisterDelayedFunction ([] { injector::MakeNOP (0x469fb5, 2); });

    this->ResetSaveData ();
    this->InitShuffledMissionOrder ();

    Logger::GetLogger ()->LogMessage ("Intialised MissionRandomizer");
}

/*******************************************************/
void
MissionRandomizer::InitShuffledMissionOrder ()
{
    auto config = ConfigManager::GetInstance ()->GetConfigs ().missions;
    mShuffledOrder.clear ();
    if (!config.shufflingEnabled)
        return;

    std::mt19937 engine{mSaveInfo.randomSeed};

    std::vector<uint8_t> remainingMissions;
    int                  index = START_MISSIONS;
    for (auto i : mSaveInfo.missionStatus.data)
        {
            for (int j = 0; j < i; j++)
                remainingMissions.push_back (index);
            index++;
        }

    index = START_MISSIONS;
    for (auto i : mSaveInfo.missionStatus.data)
        {
            printf ("Index: %d, Size: %d\n", index, i);
            for (int j = 0; j < i; j++)
                {
                    if (remainingMissions.size () <= 0)
                        continue;

                    std::uniform_int_distribution<unsigned int> dist{
                        0, remainingMissions.size () - 1};

                    auto randomMission = dist (engine);
                    mShuffledOrder[index].push_back (
                        remainingMissions[randomMission]);

                    remainingMissions.erase (remainingMissions.begin ()
                                             + randomMission);
                }
            index++;
        }
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
