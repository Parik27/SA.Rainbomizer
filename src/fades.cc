#include "fades.hh"
#include "base.hh"
#include "functions.hh"
#include "injector/calling.hpp"

/*******************************************************/
void
FadesManager::HandleFades ()
{
    static int prevFadeValue = -1;
    int        fadeValue     = injector::ReadMemory<uint8_t> (0xC3EFAB);

    if (prevFadeValue != fadeValue && fadeValue == 255)
        {
            for (const auto &i : GetFadeCallbacks ())
                i ();
        }

    prevFadeValue = fadeValue;

    HookManager::CallOriginal<injector::cstd<void ()>, 0x53EB9D> ();
}

/*******************************************************/
void
FadesManager::Initialise ()
{
    static bool bInitialised = false;
    if (std::exchange (bInitialised, true))
        return;

    RegisterHooks ({{HOOK_CALL, 0x53EB9D, (void *) HandleFades}});
}
