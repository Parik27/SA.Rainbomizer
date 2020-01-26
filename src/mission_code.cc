#include "missions.hh"
#include "functions.hh"
#include "scrpt.hh"
#include "autosave.hh"
#include "logger.hh"

/*******************************************************/
void
InsertRaceJumpAt(unsigned char* data, int index)
{
    data = Scrpt::CreateOpcode (0x004, "= const", data, GlobalVar (352), index);
    data = Scrpt::CreateOpcode (0x004, "= const", data, GlobalVar (121), 0);
    data = Scrpt::CreateOpcode (0x1096, "jump_to_mission", data, 35);
}

/*******************************************************/
void
HighStakesStartFix (MissionRandomizer *rand, unsigned char* data)
{
    rand->SetContinuedMission (35);
    rand->SetCorrectedMissionStatusIndex (35, 36);

    Scrpt::CreateOpcode (0x2, "jmp", data + 4999, -5008);

    InsertRaceJumpAt (data + 5079, 0);
}

/*******************************************************/
void
WuZiMuStartFix (MissionRandomizer *rand, unsigned char *data, bool fml = false)
{
    rand->SetContinuedMission (35);
    rand->SetCorrectedMissionStatusIndex (35, 48);

    int offset = fml ? 76 : 2037;
    Scrpt::CreateOpcode (0x002, "jmp", data + 54, -offset);

    InsertRaceJumpAt (data + 315, 7);
    InsertRaceJumpAt (data + 2208, 8);
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
JizzyStartFix (MissionRandomizer *rand, unsigned char *data, bool cutscene)
{
    int offset = (cutscene) ? 2896 : 2875;
    Scrpt::CreateOpcode(0x2, "jmp", data + 2857, -offset);
}

/*******************************************************/
void
MissionRandomizer::ApplyMissionStartSpecificFixes (unsigned char *data)
{
    switch (this->mRandomizedMissionNumber)
        {
        case 36: HighStakesStartFix (this, data); break;
        case 48:
            WuZiMuStartFix (this, data, mSaveInfo.missionStatus[48] == 2);
            break;
        case 59:
            JizzyStartFix (this, data, mSaveInfo.missionStatus[59] == 2);
            break;
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
