#pragma once

#include <vector>
#include <functional>

class FadesManager
{

    static auto &
    GetFadeCallbacks ()
    {
        static std::vector<std::function<void ()>> mFadeCallbacks;
        return mFadeCallbacks;
    }

public:
    static void HandleFades ();
    static void Initialise ();
    static void
    AddFadeCallback (std::function<void ()> function)
    {
        Initialise ();
        GetFadeCallbacks ().push_back (function);
    };
};
