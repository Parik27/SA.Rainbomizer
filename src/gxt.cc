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

GxtRandomizer *GxtRandomizer::mInstance = nullptr;

/*******************************************************/
void
GxtRandomizer::Initialise ()
{
    if (!ConfigManager::GetInstance ()->GetConfigs ().lang.enabled)
        return;
    
    Logger::GetLogger ()->LogMessage ("Intialised GxtRandomizer");

    RegisterHooks ({{HOOK_JUMP, 0x6A0050, (void *) GetTextHook}});
    InitialiseStringTable ();
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
void
GxtRandomizer::InitialiseStringTable ()
{
    for (auto &p : std::filesystem::recursive_directory_iterator ("text/"))
        {
            if (p.is_regular_file () && p.path ().has_extension ()
                && p.path ().extension () == ".gxt")
                {
                    std::ifstream file (p.path ().string (), std::ios::binary);
                    AddGxtFile (file);
                }
        }
}

/*******************************************************/
void
GxtRandomizer::AddGxtFile (std::istream &i)
{
    {
        char buf[2];
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
    auto &   config  = ConfigManager::GetInstance ()->GetConfigs ().lang;
    uint32_t crcHash = CKeyGen::GetUppercaseKey (key);

    if (!m_StringTable.count (crcHash))
        return (char *) "";

    auto &table = m_StringTable[crcHash];

    if (config.MaxTimeBeforeTextChange == 0
        || config.MinTimeBeforeTextChange == 0)
        {
            return (char *) GetRandomElement (table).data ();
        }

    return table[int (crcHash
                      + time (NULL)
                            / ((crcHash
                                % (std::max (
                                    config.MaxTimeBeforeTextChange
                                        - config.MinTimeBeforeTextChange,
                                    1)))
                               + std::max (config.MinTimeBeforeTextChange, 1)))
                 % table.size ()]
        .data ();
}
