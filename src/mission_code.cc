#include "missions.hh"
#include "functions.hh"
#include "scrpt.hh"
#include "autosave.hh"
#include "logger.hh"

/*******************************************************/
void
InsertRaceJumpAt (unsigned char *data, int index)
{
    data = Scrpt::CreateOpcode (0x004, "= const", data, GlobalVar (352), index);
    data = Scrpt::CreateOpcode (0x004, "= const", data, GlobalVar (121), 0);
    data = Scrpt::CreateOpcode (OPCODE_REPLACE_MISSION, "", data, 35);
}

/*******************************************************/
void
HighStakesStartFix (MissionRandomizer *rand, unsigned char *data)
{
    rand->SetContinuedMission (35);
    rand->SetCorrectedMissionStatusIndex (35, 36);

    Scrpt::CreateOpcode (0x2, "jmp", data + 4999, -5008);

    InsertRaceJumpAt (data + 5079, 0);
}

/*******************************************************/
void
WuZiMuStartFix (MissionRandomizer *rand, unsigned char *data, bool wzm = false)
{
    rand->SetContinuedMission (35);
    rand->SetCorrectedMissionStatusIndex (35, 48);

    int offset = wzm ? 76 : 2037;
    Scrpt::CreateOpcode (0x002, "jmp", data + 54, -offset);

    InsertRaceJumpAt (data + 315, 7);
    InsertRaceJumpAt (data + 2208, 8);
}

/*******************************************************/
void
JizzyStartFix (unsigned char *data, bool cutscene)
{
    int offset = (cutscene) ? 2896 : 2875;
    Scrpt::CreateOpcode (0x2, "jmp", data + 2857, -offset);
}

/*******************************************************/
void
HousePartyStartFix (unsigned char *data, bool cutscene)
{
    int offset = (cutscene) ? 421 : 609;
    Scrpt::CreateOpcode (0x2, "jmp", data + 403, -offset);

    offset = (cutscene) ? 1099 : 1112;
    Scrpt::CreateOpcode (0x2, "jmp", data + 1081, -offset);
}

/*******************************************************/
void
GreenSabreStartFix (unsigned char *data)
{
    Scrpt::CreateNop (data, 19773, 19780);
    Scrpt::CreateNop (data, 19791, 19811);
    Scrpt::CreateNop (data, 19567, 19587);
    Scrpt::CreateNop (data, 22262, 22275);
}

/*******************************************************/
void
CustomsFastTrackStartFix (unsigned char *data)
{
    data += 25110;
    data = Scrpt::CreateOpcode (0x6, "set_lvar", data, LocalVar (36), 1);
    data = Scrpt::CreateOpcode (0x51, "return", data);
}

/*******************************************************/
void
FixRaces (MissionRandomizer *rand, unsigned char *data)
{
    Scrpt::CreateNop (data, 19642, 19649);
    Scrpt::CreateNop (data, 19649, 19706);
}

/*******************************************************/
void
CatalinaStartFix (unsigned char *data, std::vector<int> offsets)
{
    data[offsets[random (offsets.size () - 1)] + 6] = ScriptSpace[64];
}

/*******************************************************/
void
RyderStartFix (unsigned char *data)
{
    Scrpt::CreateNop (data, 1391, 1469);
    Scrpt::CreateNop (data, 11509, 11522);
}

/*******************************************************/
void
NewModelArmyStartFix (unsigned char *data)
{
    auto _data = Scrpt::CreateOpcode (0x50, "gosub", data + 26926, -27317);
    Scrpt::CreateOpcode (0x2, "jump", _data, -27062);
    Scrpt::CreateOpcode (0x2, "jump", data + 27086, -27224);
}

/*******************************************************/
void
MaddDoggRhymesFix (unsigned char* data)
{
    if (CEnterExit::GetInteriorStatus ("MADDOGS"))
        Scrpt::CreateNop (data, 10596, 10622);
}

/*******************************************************/
void
LosDesperadosFix (unsigned char* data)
{
    Scrpt::CallOpcode(0x076C, "set_zone_gang_density", "GAN1", 1, 25);
    Scrpt::CallOpcode(0x076C, "set_zone_gang_density", "GAN2", 1, 25);
}

/*******************************************************/
void
MissionRandomizer::ApplyMissionStartSpecificFixes (unsigned char *data)
{
    switch (this->mRandomizedMissionNumber)
        {
        case 48:
            WuZiMuStartFix (this, data, mSaveInfo.missionStatus[48] == 2);
            break;
        case 59: JizzyStartFix (data, mSaveInfo.missionStatus[59] == 2); break;
        case 34:
            HousePartyStartFix (data, mSaveInfo.missionStatus[34] == 2);
            break;
        case 41: CatalinaStartFix (data, {22837, 23853, 24305, 24580}); break;
        case 42: CatalinaStartFix (data, {28469, 28849, 29055, 29174}); break;
        case 36: HighStakesStartFix (this, data); break;
        case 38: GreenSabreStartFix (data); break;
        case 69: CustomsFastTrackStartFix (data); break;
        case 35: FixRaces (this, data); break;
        case 12: RyderStartFix (data); break;
        case 74: NewModelArmyStartFix (data); break;
        case 32: MaddDoggRhymesFix(data); break;
        case 109: LosDesperadosFix(data); break;
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
MissionRandomizer::ApplyMissionFailFixes()
{
    switch (this->mOriginalMissionNumber)
        {
        // King in Exile
        case 45:
            ScriptSpace[719] = 1;
            break;
        }
}

/*******************************************************/
void
MissionRandomizer::ApplyMissionSpecificFixes (uint8_t *data)
{
    switch (this->mOriginalMissionNumber)
        {

        // High Stakes
        case 36:
            data += 5520;
            data = Scrpt::CreateOpcode (0x8, "incrmt_var", data,
                                        GlobalVar (457), 1);
            data = Scrpt::CreateOpcode (0x318, "set_latest_mission_passed",
                                        data, "CESAR_1");
            data = Scrpt::CreateOpcode (0x164, "disable_marker", data,
                                        GlobalVar (440));
            data = Scrpt::CreateOpcode (0x51, "return", data);
            break;

        // Green Sabre
        case 38:
            data += 22051;
            data = Scrpt::CreateOpcode (0x629, "change_int_stat", data, 181, 1);
            data = Scrpt::CreateOpcode (0x777, "delete_things", data,
                                        "BARRIERS1");
            data = Scrpt::CreateOpcode (0x51, "return", data);
            break;

        // Wu Zi Mu / Farewell My Love
        case 48:
            if (mRandomizedMissionNumber != 48)
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
            data += 25743;
            data = Scrpt::CreateOpcode (0x2, "jump", data, -30732);

            break;

        // New Model Army
        case 74:
            Scrpt::CreateNop (data, 27236, 27254);
            Scrpt::CreateOpcode (0x2, "jump", data + 27317, -26926);
            Scrpt::CreateOpcode (0x51, "return", data + 27193);
            break;

        // Outrider
        case 60:

            // remove code setting max wanted level to 0
            Scrpt::CreateNop (data, 19211, 19216);
            break;
        }
}
