#pragma once

class TimeCycleRandomizer
{
    static TimeCycleRandomizer *mInstance;

    TimeCycleRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for TimeCycleRandomizer.
    static TimeCycleRandomizer *GetInstance ();

    static inline struct Config
    {
        bool RandomizeTimeCycle;
        bool ChangeOnFade;
        bool RandomizeWeather;
        int RandomTimecycleOdds;
    } m_Config;

    /// Initialises Hooks/etc.
    void Initialise ();
};
