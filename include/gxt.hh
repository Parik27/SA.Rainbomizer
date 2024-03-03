#pragma once

struct CText;

#include <istream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

class GxtRandomizer
{
    static GxtRandomizer *mInstance;

    inline static std::unordered_map<uint32_t, std::vector<std::string>>
        m_StringTable;

    static char *__fastcall GetTextHook (CText *text, void *edx, char *key);

    void ReadDatEntry (std::istream &i, uint32_t crc32);
    void ReadKeyTable (std::istream &i);
    void ReadStringTable (std::istream &i, uint32_t);
    bool InitialiseStringTable ();
    void AddGxtFile (std::istream &i);

    GxtRandomizer (){};
    static void DestroyInstance ();

public:
    static inline struct Config
    {
        int MinTime;
        int MaxTime;
    } m_Config;

    inline static char  lastZone[8];
    inline static char *randomZoneText;

    inline static char  lastCar[8];
    inline static char *randomCarText;

    /// Returns the static instance for GxtRandomizer.
    static GxtRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();
};
