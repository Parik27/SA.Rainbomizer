#pragma once

struct CText;

const char *__fastcall GetNewCustomPlateText (void *thisInfo, void *edx);
void GetNewPlateText (char *buf, int len);
void __fastcall InitialiseText (CText *text, void *edx, char a2);

class LicensePlateRandomizer
{
    static LicensePlateRandomizer *mInstance;

    LicensePlateRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for LicensePlateRandomizer.
    static LicensePlateRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();
};
