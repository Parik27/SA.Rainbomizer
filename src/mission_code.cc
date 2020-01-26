#include "missions.hh"
#include "functions.hh"
#include "scrpt.hh"
#include "autosave.hh"

/*******************************************************/
void
HighStakesStartFix (MissionRandomizer *rand)
{
    rand->SetContinuedMission (35);
    rand->SetCorrectedMissionStatusIndex (35, 36);

    // Reset $457 once the mission ends
    rand->AddToMissionCleanup (
        [original_status (ScriptSpace[457])] {
            ScriptSpace[457] = (ScriptSpace[457] == 0) ? original_status : 1;
        },
        true, true, [] { return ScriptSpace[2336] != 1; });

    ScriptSpace[457] = 0;
    // Create the 'cesar' thread
    if (!CRunningScripts::CheckForRunningScript ("cesar"))
        {
            Scrpt::CallOpcode (0x4F, "create_thread", 64462);
            // Terminates cesar when the mission ends
            rand->AddToMissionCleanup (
                [] { Scrpt::CallOpcode (0x459, "terminate_thread", "cesar"); },
                true, true, [] { return ScriptSpace[2336] != 1; });
        }
}

/*******************************************************/
void
WuZiMuStartFix (MissionRandomizer *rand, bool fml = false)
{
    rand->SetContinuedMission (35);
    rand->SetCorrectedMissionStatusIndex (35, 48);

    rand->AddToMissionCleanup (
        [original (ScriptSpace[64])] {
            if (ScriptSpace[64] - 10 == 1)
                ScriptSpace[64] = original + 1;
            else if (ScriptSpace[64] - 10 == 0)
                ScriptSpace[64] = original;
        },
        true, true,
        [rand] {
            return ScriptSpace[2196] != 1 && rand->mOriginalMissionNumber != 48;
        });

    ScriptSpace[64] = 10;

    // Reset $457 once the mission ends
    rand->AddToMissionCleanup (
        [original (ScriptSpace[492]), fml] {
            if (ScriptSpace[492] == (fml ? 9 : 4))
                ScriptSpace[492] = original;
        },
        true, true,
        [rand] {
            return ScriptSpace[2196] != 1 && rand->mOriginalMissionNumber != 48;
        });

    if (rand->mOriginalMissionNumber != 48)
        ScriptSpace[492] = fml ? 5 : 0;

    // Create the 'bcesar' thread

    if (!CRunningScripts::CheckForRunningScript ("bcesar"))
        {
            Scrpt::CallOpcode (0x4F, "create_thread", 66700);
            // Terminates cesar when the mission ends
            rand->AddToMissionCleanup (
                [] { Scrpt::CallOpcode (0x459, "terminate_thread", "bcesar"); },
                true, true, [] { return ScriptSpace[2196] != 1; });
        }
}

/*******************************************************/
void
MissionRandomizer::HandleGoSubAlternativeForMission (int index)
{
    switch (index)
        {
        case 112:
            Scrpt::CallOpcode (0x68D, "get_camera_pos_to", GlobalVar (69),
                               GlobalVar (70), GlobalVar (71));
            Scrpt::CallOpcode (0x9, "add_float", GlobalVar (71), 1.5f);
            Scrpt::CallOpcode (0xa1, "Actor.PutAt", GlobalVar (3),
                               GlobalVar (69), GlobalVar (70), GlobalVar (71));
        }
}

/*******************************************************/
void
MissionRandomizer::ApplyMissionStartSpecificFixes (unsigned char *data)
{
    switch (this->mRandomizedMissionNumber)
        {
        case 36: HighStakesStartFix (this); break;
        case 48: WuZiMuStartFix (this, mSaveInfo.missionStatus[48] == 1); break;
        }
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
