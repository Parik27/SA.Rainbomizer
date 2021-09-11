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

#pragma once

/* Base - contains classes for setting up hooks */
#include <cstdint>
#include <vector>
#include <functional>
#include <cstring>
#include <unordered_map>
#include <string>

enum eHookType
{
    HOOK_JUMP,
    HOOK_CALL
};

struct HookProperties
{
    eHookType type;
    uintptr_t src;
    void *    dest;
};

struct GamePathA
{
    char _path[160 + 1];
    char _temp_path[160 + 1];

    GamePathA ();
};

char *      GetGameDirRelativePathA (const char *subpath);
std::string GetRainbomizerFileName (std::string name, std::string subdirs = "");
FILE *      OpenRainbomizerFile (std::string name, std::string mode,
                                 std::string subdirs = "");
FILE *      GetRainbomizerDataFile (const std::string &name,
                                    const std::string &mode = "r");

bool VerifyGameVersion ();

/*******************************************************/
class HookManager
{
    static int InstallDelayedHooks ();

    static HookManager *                   mInstance;
    std::unordered_map<uint32_t, uint32_t> mHooks;
    std::vector<HookProperties>            mDelayedHooks;
    std::vector<std::function<void ()>>    mDelayedFuncs;

    HookManager (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for HookManager.
    static HookManager *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();

    /// Creates hooks based on HookProperties
    void RegisterHooks (std::vector<HookProperties> hooks);
    void RegisterDelayedFunction (std::function<void ()> func);
    void RegisterDelayedHooks (std::vector<HookProperties> hooks);

    static int GetOriginalCall (int call);

    template <typename T, int addr, typename... Args>
    static void
    CallOriginal (Args... args)
    {
        static int branch = GetOriginalCall (addr);
        if (branch)
            T::call (branch, args...);
    }

    template <typename T, int addr, typename... Args>
    static auto
    CallOriginalAndReturn (std::function<typename T::result_type ()> fallback,
                           Args... args)
    {
        static int branch = GetOriginalCall (addr);
        if (branch)
            return T::call (branch, args...);
        return fallback ();
    }

    template <typename T, int addr, typename... Args>
    static auto
    CallOriginalAndReturn (typename T::result_type fallback, Args... args)
    {
        int branch = GetOriginalCall (addr);
        if (branch)
            return T::call (branch, args...);
        return fallback;
    }
};

void RegisterHooks (std::vector<HookProperties> hooks);
void RegisterDelayedFunction (std::function<void ()> func);
void RegisterDelayedHooks (std::vector<HookProperties> hooks);

/// Unprotects the entire module
void UnProtectInstance ();

struct _EXCEPTION_POINTERS;

class ExceptionManager
{
    static ExceptionManager *mManager;
    static long __stdcall RunExceptionHandler (_EXCEPTION_POINTERS *ep);

    ExceptionManager (){};
    std::vector<std::function<void (_EXCEPTION_POINTERS *ep)>>
        mExceptionHandlers;

    static void DestroyExceptionManager ();

public:
    /// Registers a new Exception Handler. These are executed in order in a case
    /// of a crash
    void
    RegisterHandler (std::function<void (_EXCEPTION_POINTERS *ep)> handler);

    /// Initiates the Exception Manager
    void RegisterExceptionManager ();

    /// Returns/Creates the Exception Manager
    static ExceptionManager *GetExceptionManager ();
};

/*******************************************************/
constexpr char
NormaliseChar (const char c)
{
    if (c >= 'A' && c <= 'Z')
        return c + ('a' - 'A');

    else if (c == '\\')
        return '/';

    return c;
}

/*******************************************************/
template <unsigned int address, typename... Args>
void
Call (Args... args)
{
    reinterpret_cast<void (__cdecl *) (Args...)> (address) (args...);
}

/*******************************************************/
template <typename Ret, unsigned int address, typename... Args>
Ret
CallAndReturn (Args... args)
{
    return reinterpret_cast<Ret (__cdecl *) (Args...)> (address) (args...);
}

/*******************************************************/
template <unsigned int address, typename C, typename... Args>
void
CallMethod (C _this, Args... args)
{
    reinterpret_cast<void (__thiscall *) (C, Args...)> (address) (_this,
                                                                  args...);
}

/*******************************************************/
template <typename Ret, unsigned int address, typename C, typename... Args>
Ret
CallMethodAndReturn (C _this, Args... args)
{
    return reinterpret_cast<Ret (__thiscall *) (C, Args...)> (
        address) (_this, args...);
}

/*******************************************************/
template <unsigned int tableIndex, typename C, typename... Args>
void
CallVirtualMethod (C _this, Args... args)
{
    reinterpret_cast<void (__thiscall *) (C, Args...)> (
        (*reinterpret_cast<void ***> (_this))[tableIndex]) (_this, args...);
}

/*******************************************************/
template <typename Ret, unsigned int tableIndex, typename C, typename... Args>
Ret
CallVirtualMethodAndReturn (C _this, Args... args)
{
    return reinterpret_cast<Ret (__thiscall *) (C, Args...)> (
        (*reinterpret_cast<void ***> (_this))[tableIndex]) (_this, args...);
}