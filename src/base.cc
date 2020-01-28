/*
    Rainbomizer - A (probably fun) Grand Theft Auto San Andreas Mod that
                  randomizes stuff
    Copyright (C) 2019 - Parik

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

 */

/* Base - contains classes for setting up hooks */
#include "base.hh"
#include <windows.h>
#include "logger.hh"
#include <memory>
#include "injector/injector.hpp"
#include "injector/calling.hpp"
#include <algorithm>
#include <unordered_map>

typedef injector::memory_pointer_raw memptr_t;
HookManager *                        HookManager::mInstance = nullptr;

/*******************************************************/
// Stolen shamelessly from https://stackoverflow.com/a/26221725
template <typename... Args>
std::string
string_format (const std::string &format, Args... args)
{
    size_t size = 1 + snprintf (nullptr, 0, format.c_str (), args...);
    std::unique_ptr<char[]> buf (new char[size]);
    snprintf (buf.get (), size, format.c_str (), args...);
    return std::string (buf.get (), buf.get () + size);
}
/*******************************************************/
void
HookManager::Initialise ()
{
    Logger::GetLogger ()->LogMessage ("Initialised Delayed Hooks");
    RegisterHooks (
        {{HOOK_CALL, 0x5BFA90, (void *) &HookManager::InstallDelayedHooks}});

    Logger::GetLogger ()->LogMessage ("Intialised HookManager");
}

/*******************************************************/
std::string
GetRainbomizerFileName (std::string name)
{
    return GetGameDirRelativePathA (("rainbomizer/" + name).c_str ());
}

/*******************************************************/
void
CreateRainbomizerFolder ()
{
    CreateDirectory (GetRainbomizerFileName ("").c_str (), NULL);
}

/*******************************************************/
FILE *
OpenRainbomizerFile (std::string name, std::string mode)
{
    return fopen (GetRainbomizerFileName (name).c_str (), mode.c_str ());
}

/*******************************************************/
void
HookManager::DestroyInstance ()
{
    if (HookManager::mInstance)
        delete HookManager::mInstance;
}

/*******************************************************/
HookManager *
HookManager::GetInstance ()
{
    if (!HookManager::mInstance)
        {
            HookManager::mInstance = new HookManager ();
            atexit (&HookManager::DestroyInstance);
        }
    return HookManager::mInstance;
}

/*******************************************************/
void
HookManager::RegisterHooks (std::vector<HookProperties> hooks)
{
    for (auto hook : hooks)
        {
            memptr_t original;
            switch (hook.type)
                {
                case HOOK_JUMP:
                    original = injector::MakeJMP (hook.src, hook.dest);
                    Logger::GetLogger ()->LogMessage (
                        string_format ("JUMP: Hooking %X => %X", hook.src,
                                       hook.dest));
                    break;

                case HOOK_CALL:
                    original = injector::MakeCALL (hook.src, hook.dest);
                    Logger::GetLogger ()->LogMessage (
                        string_format ("CALL: Hooking %X => %X", hook.src,
                                       hook.dest));
                    break;
                }

            mHooks[hook.src] = original.as_int ();
        }
}

/*******************************************************/
int
HookManager::GetOriginalCall (int newCall)
{
    auto hookManager = HookManager::GetInstance ();
    return hookManager->mHooks[newCall];
};

/*******************************************************/
int
HookManager::InstallDelayedHooks ()
{
    auto hookManager = HookManager::GetInstance ();
    ::RegisterHooks (hookManager->mDelayedHooks);
    for (auto i : hookManager->mDelayedFuncs)
        i ();

    return CallOriginalAndReturn<injector::cstd<int ()>, 0x5BFA90> (0);
}

/*******************************************************/
void
HookManager::RegisterDelayedFunction (std::function<void ()> func)
{
    mDelayedFuncs.push_back (func);
}

/*******************************************************/
void
HookManager::RegisterDelayedHooks (std::vector<HookProperties> hooks)
{
    for (auto i : hooks)
        mDelayedHooks.push_back (i);
}

/*******************************************************/
void
RegisterHooks (std::vector<HookProperties> hooks)
{
    HookManager::GetInstance ()->RegisterHooks (hooks);
}

/*******************************************************/
void
RegisterDelayedHooks (std::vector<HookProperties> hooks)
{
    HookManager::GetInstance ()->RegisterDelayedHooks (hooks);
}

/*******************************************************/
void
RegisterDelayedFunction (std::function<void ()> func)
{
    HookManager::GetInstance ()->RegisterDelayedFunction (func);
}

/*******************************************************/
void
UnProtectInstance ()
{
    auto              hExecutableInstance = (size_t) GetModuleHandle (NULL);
    IMAGE_NT_HEADERS *ntHeader
        = (IMAGE_NT_HEADERS *) (hExecutableInstance
                                + ((IMAGE_DOS_HEADER *) hExecutableInstance)
                                      ->e_lfanew);
    SIZE_T size = ntHeader->OptionalHeader.SizeOfImage;
    DWORD  oldProtect;
    VirtualProtect ((VOID *) hExecutableInstance, size, PAGE_EXECUTE_READWRITE,
                    &oldProtect);
}

ExceptionManager *ExceptionManager::mManager = nullptr;

/*******************************************************/
void
ExceptionManager::RegisterHandler (
    std::function<void (_EXCEPTION_POINTERS *ep)> handler)
{
    mExceptionHandlers.push_back (handler);
}

/*******************************************************/
long __stdcall ExceptionManager::RunExceptionHandler (_EXCEPTION_POINTERS *ep)
{
    auto logger = Logger::GetLogger ();
    logger->LogMessage ("Game has crashed");

    DWORD exceptionCode    = ep->ExceptionRecord->ExceptionCode;
    PVOID exceptionAddress = ep->ExceptionRecord->ExceptionAddress;

    // Print Exception Type and address
    logger->LogMessage (string_format ("Exception Type: 0x%X", exceptionCode));
    logger->LogMessage (
        string_format ("Exception Address: 0x%X", exceptionAddress));

    // Print the memory address that was accessed too.
    if (exceptionCode == EXCEPTION_ACCESS_VIOLATION
        || exceptionCode == EXCEPTION_IN_PAGE_ERROR)
        logger->LogMessage (
            string_format ("Inaccessible memory address: 0x%X",
                           ep->ExceptionRecord->ExceptionInformation[1]));

    for (auto callback : GetExceptionManager ()->mExceptionHandlers)
        callback (ep);

    return EXCEPTION_CONTINUE_SEARCH;
}

/*******************************************************/
void
ExceptionManager::RegisterExceptionManager ()
{
    SetUnhandledExceptionFilter (&ExceptionManager::RunExceptionHandler);
}

/*******************************************************/
void
ExceptionManager::DestroyExceptionManager ()
{
    if (mManager)
        delete mManager;
}

/*******************************************************/
ExceptionManager *
ExceptionManager::GetExceptionManager ()
{
    if (!mManager)
        {
            mManager = new ExceptionManager ();
            atexit (&ExceptionManager::DestroyExceptionManager);
        }

    return mManager;
}

/*******************************************************/
GamePathA::GamePathA ()
{
    _path[0]  = '\0';
    HMODULE h = NULL;
    h         = GetModuleHandleA (NULL);
    GetModuleFileNameA (h, _path, MAX_PATH);
    char *bslp = strrchr (_path, '\\');
    char *fslp = strrchr (_path, '/');
    char *slp  = (std::max) (bslp, fslp);
    if (slp)
        slp[1] = '\0';
}

/*******************************************************/
char *
GetGameDirRelativePathA (const char *subpath)
{
    static GamePathA gamePath;
    strcpy (gamePath._temp_path, gamePath._path);
    strcat (gamePath._temp_path, subpath);
    return gamePath._temp_path;
}
