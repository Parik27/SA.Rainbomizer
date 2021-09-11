#include "gxt.hh"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
#include <stdio.h>
#include <string.h>
#include <string>
#include <time.h>
#include <cmath>
#include "base.hh"
#include "config.hh"
#include "functions.hh"
#include "logger.hh"
#include "injector/calling.hpp"
#include <chrono>
#include "riot.hh"

GxtRandomizer *GxtRandomizer::mInstance = nullptr;

/*******************************************************/
void
GxtRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig (
            "LanguageRandomizer",
            std::pair ("MinTimeBeforeTextChange", &m_Config.MinTime),
            std::pair ("MaxTimeBeforeTextChange", &m_Config.MaxTime)))
        return;

    Logger::GetLogger ()->LogMessage ("Intialised GxtRandomizer");

    if (InitialiseStringTable ())
        RegisterHooks ({{HOOK_JUMP, 0x6A0050, (void *) GetTextHook}});
}

/*******************************************************/
void
GxtRandomizer::DestroyInstance ()
{
    if (GxtRandomizer::mInstance)
        delete GxtRandomizer::mInstance;
}

/*******************************************************/
GxtRandomizer *
GxtRandomizer::GetInstance ()
{
    if (!GxtRandomizer::mInstance)
        {
            GxtRandomizer::mInstance = new GxtRandomizer ();
            atexit (&GxtRandomizer::DestroyInstance);
        }
    return GxtRandomizer::mInstance;
}

/*******************************************************/
bool
GxtRandomizer::InitialiseStringTable ()
{
    bool initialised = false;
    for (auto &p : std::filesystem::recursive_directory_iterator (
             GetGameDirRelativePathA ("text/")))
        {
            if (p.is_regular_file () && p.path ().has_extension ()
                && p.path ().extension () == ".gxt")
                {
                    std::ifstream file (p.path ().string (), std::ios::binary);
                    AddGxtFile (file);
                    if (p.path ().filename () == "american.gxt")
                        {
                            for (int i = 0; i < 3; i++)
                                {
                                    std::ifstream fileExtra (
                                        p.path ().string (), std::ios::binary);
                                    AddGxtFile (fileExtra);
                                }
                        }

                    initialised = true;
                }
        }

    return initialised;
}

/*******************************************************/
void
GxtRandomizer::AddGxtFile (std::istream &i)
{
    {
        char buf[4];
        i.read (buf, 4);
    }

    while (i.good ())
        {
            char     header[4];
            uint32_t size;

            i.read (header, 4);
            i.read ((char *) &size, 4);

            if (!strncmp (header, "TABL", 4))
                return ReadStringTable (i, size / 12);
            else
                {
                    i.seekg (size, std::ios_base::cur);
                }
        }
}

/*******************************************************/
void
GxtRandomizer::ReadStringTable (std::istream &in, uint32_t numEntries)
{
    struct GxtTable
    {
        char     entryName[8];
        uint32_t nOffset;
    } table;

    for (int i = 0; i < numEntries; i++)
        {
            in.read ((char *) &table, sizeof (GxtTable));
            size_t returnAddr = in.tellg ();

            in.seekg (table.nOffset
                      + (!strncmp (table.entryName, "MAIN", 8) ? 0 : 8));
            ReadKeyTable (in);

            in.seekg (returnAddr);
        }
}

/*******************************************************/
void
GxtRandomizer::ReadKeyTable (std::istream &in)
{
    char     header[4];
    uint32_t size;

    in.read (header, 4);
    in.read ((char *) &size, 4);

    auto datOffset = size_t (in.tellg ()) + size + 8;

    if (!strncmp (header, "TKEY", 4))
        {
            for (int i = 0; i < size / 8; i++)
                {
                    struct GxtKeyEntry
                    {
                        uint32_t entryOffset;
                        uint32_t crcHash;
                    } key;

                    in.read ((char *) &key, sizeof (key));

                    size_t returnAddr = in.tellg ();
                    in.seekg (key.entryOffset + datOffset);

                    ReadDatEntry (in, key.crcHash);
                    in.seekg (returnAddr);
                }
        }
}

/*******************************************************/
void
GxtRandomizer::ReadDatEntry (std::istream &in, uint32_t crc32)
{
    std::string str;
    std::getline (in, str, '\0');

    m_StringTable[crc32].push_back (str);
}

/*******************************************************/
char *__fastcall GxtRandomizer::GetTextHook (CText *text, void *edx, char *key)
{
    bool pFound = false;
    if (CGame::bMissionPackGame)
        {
            // CKeyArray::Search
            char *result
                = CallMethodAndReturn<char *, 0x6A0000> (text, key, &pFound);
            if (!pFound)
                return key;
        }

    int typeOfZoneKey = 0;
    int typeOfVehKey  = 0;

    if (std::string (key) != std::string (RiotRandomizer::previousZone)
        && std::string (key) != std::string (GxtRandomizer::lastZone))
        {
            typeOfZoneKey = 0; // This text is not zone-related
        }
    else if (std::string (key) == std::string (RiotRandomizer::previousZone)
             && std::string (key) != std::string (GxtRandomizer::lastZone))
        {
            typeOfZoneKey = 1; // Zone changed, need new random zone text
        }
    else if (std::string (key) == std::string (RiotRandomizer::previousZone)
             && std::string (key) == std::string (GxtRandomizer::lastZone))
        {
            typeOfZoneKey = 2; // Zone text but zone is the same
        }

    char currentCarKey[8];
    if (FindPlayerVehicle ())
        {
            uint16_t playerCarModel = FindPlayerVehicle ()->m_nModelIndex;
            CVehicleModelInfo *currentCarPtr
                = injector::ReadMemory<CVehicleModelInfo *> (
                    0xA9B0C8 + (4 * playerCarModel));
            for (int i = 0; i < 8; i++)
                {
                    currentCarKey[i] = currentCarPtr->m_szGameName[i];
                }
            if (std::string (key) != std::string (GxtRandomizer::lastCar)
                && std::string (key) == std::string (currentCarKey))
                {
                    typeOfVehKey
                        = 1; // Vehicle text has changed, need new random text
                }
            else if (std::string (key) == std::string (GxtRandomizer::lastCar)
                     && std::string (key) == std::string (currentCarKey))
                {
                    typeOfVehKey = 2; // Vehicle text unchanged
                }
        }
    else
        {
            for (int i = 0; i < 8; i++)
                {
                    GxtRandomizer::lastCar[i] = 'z';
                }
        }

    for (int i = 0; i < 8; i++)
        {
            GxtRandomizer::lastZone[i] = RiotRandomizer::previousZone[i];
        }

    uint32_t crcHash = CKeyGen::GetUppercaseKey (key);

    if (!m_StringTable.count (crcHash))
        return (char *) "";

    auto &table = m_StringTable[crcHash];

    if (typeOfZoneKey == 1)
        {
            GxtRandomizer::randomZoneText
                = (char *) GetRandomElement (table).data ();
        }

    if (typeOfZoneKey == 1 || typeOfZoneKey == 2)
        {
            return GxtRandomizer::randomZoneText;
        }

    if (typeOfVehKey == 1)
        {
            GxtRandomizer::randomCarText
                = (char *) GetRandomElement (table).data ();
            for (int i = 0; i < 8; i++)
                {
                    GxtRandomizer::lastCar[i] = currentCarKey[i];
                }
        }

    if (typeOfVehKey == 1 || typeOfVehKey == 2)
        {
            return GxtRandomizer::randomCarText;
        }

    if (GxtRandomizer::m_Config.MaxTime == 0
        || GxtRandomizer::m_Config.MinTime == 0)
        {
            return (char *) GetRandomElement (table).data ();
        }

    return table[int (crcHash
                      + time (NULL)
                            / ((crcHash
                                % (std::max (
                                    GxtRandomizer::m_Config.MaxTime
                                        - GxtRandomizer::m_Config.MinTime,
                                    1)))
                               + std::max (GxtRandomizer::m_Config.MinTime, 1)))
                 % table.size ()]
        .data ();
}
