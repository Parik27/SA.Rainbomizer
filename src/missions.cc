#include "missions.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "functions.hh"
#include <vector>
#include "injector/injector.hpp"
#include "config.hh"

MissionRandomizer *MissionRandomizer::mInstance = nullptr;

std::vector<std::vector<int>> threadFinishes = {
    {18188}, {13692}, {14682},        {15979}, {12908}, {20304}, {23785},
    {15274}, {35970}, {32422},        {6337},  {41291}, {19361}, {40660},
    {18159}, {41389}, {23183},        {11352}, {20115}, {52043}, {15574},
    {10527}, {15510}, {15973, 15889}, {19649}, {19649}, {67765}, {20103},
    {13609}, {0},     {32513},        {30343}, {17169}, {18326}, {813, 912},
    {16397}, {11942}, {1957, 2617},   {24446}, {21029}, {18160}, {21206},
    {27975}, {8062},  {12781},        {26588}, {20684}, {14622}, {31204, 31291},
    {19158}, {12306}, {9169},         {14730}, {23169}, {12885}, {15909},
    {7659},  {26031}, {30732},        {5117},  {24495}, {17199}, {7889},
    {27224}, {14457}, {10715},        {26934}, {310},   {10459}, {21894},
    {29924}, {4434},  {30015},        {19542}, {16387}, {14129}, {319},
    {45880}, {12838}, {31283},        {12440}, {15339}, {7980},  {5058},
    {13261}, {23694}, {23003},        {18813}, {13180}, {13735}, {44675},
    {38947}, {52419}, {6740},         {12906}, {18881}, {3159},  {2997},
    {30259}, {32590}, {18535},        {57451},
};

/*******************************************************/
void __fastcall RandomizeMissionToStart (CRunningScript *scr, void *edx,
                                         short count)
{
    auto missionRandomizer = MissionRandomizer::GetInstance ();

    const int START_MISSIONS = 11;
    const int END_MISSIONS   = 112;

    scr->CollectParameters (count);
    if (ScriptParams[0] >= START_MISSIONS && ScriptParams[0] <= END_MISSIONS)
        {
            missionRandomizer->mOriginalMissionNumber
                = ScriptParams[0] - START_MISSIONS;
            ScriptParams[0] = random (END_MISSIONS, START_MISSIONS);
            missionRandomizer->mRandomizedMissionNumber
                = ScriptParams[0] - START_MISSIONS;
            missionRandomizer->mStoreNextMission = true;
        }
}

/*******************************************************/
bool
MissionRandomizer::ShouldJump (CRunningScript *scr)
{
    int currentOffset = scr->m_pCurrentIP - scr->m_pBaseIP;
    if (currentOffset != this->mPrevOffset)
        {
            short opCode = *reinterpret_cast<uint16_t *> (scr->m_pCurrentIP);
            if (opCode == 0x51 && mScriptReplaced)
                {
                    // Restore original base ip
                    scr->m_pBaseIP  = mOriginalBaseIP;
                    mScriptReplaced = false;

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
MissionRandomizer::MoveScriptToOriginalOffset (CRunningScript *scr)
{
    int *missionOffsets = (int *) 0xA444C8;
    int  baseOffset     = missionOffsets[this->mOriginalMissionNumber + 11];
    int  offset         = threadFinishes[this->mOriginalMissionNumber][0];

    FILE *scm = fopen (GetGameDirRelativePathA ("data/script/main.scm"), "rb");
    fseek (scm, baseOffset, SEEK_SET);

    mScriptReplaced = true;
    mOriginalBaseIP = scr->m_pBaseIP;

    fread (this->mTempMissionData, 1, 50000, scm);
    scr->m_pBaseIP    = this->mTempMissionData;
    scr->m_pCurrentIP = scr->m_pBaseIP + offset;

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

    // Original instruction
    (*((int *) 0xA447F4))++;
}

/*******************************************************/
CRunningScript *
StoreRandomizedScript (uint8_t *startIp)
{
    auto missionRandomizer = MissionRandomizer::GetInstance ();
    auto out = CallAndReturn<CRunningScript *, 0x464C20> (startIp);

    if (missionRandomizer->mStoreNextMission)
        missionRandomizer->mRandomizedScript = out;

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
        mTempMissionData = new unsigned char[50000];
    if (!mLocalVariables)
        mLocalVariables = new int[1024];

    RegisterHooks ({{HOOK_CALL, 0x489929, (void *) &RandomizeMissionToStart},
                    {HOOK_CALL, 0x489A7A, (void *) &StoreRandomizedScript},
                    {HOOK_CALL, 0x469FB0, (void *) &JumpOnMissionEnd},
                    {HOOK_CALL, 0x465ED1, (void *) &UpdatePCHook}});

    injector::MakeNOP (0x469FB5, 2);

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
