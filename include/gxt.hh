#pragma once

struct CText;

#include <istream>
#include <string>
#include <unordered_map>
#include <vector>

class GxtRandomizer
{
    static GxtRandomizer *mInstance;

    inline static std::unordered_map<uint32_t, std::vector<std::string>>
        m_StringTable;

    static char *__fastcall GetTextHook (CText *text, void *edx, char *key);

    void ReadDatEntry (std::istream &i, uint32_t crc32);
    void ReadKeyTable (std::istream &i);
    void ReadStringTable (std::istream &i, uint32_t);
    void InitialiseStringTable ();
    void AddGxtFile (std::istream &i);

    GxtRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for GxtRandomizer.
    static GxtRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();
};
