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

/// Creates hooks based on HookProperties
void RegisterHooks (std::vector<HookProperties> hooks);

class _EXCEPTION_POINTERS;

class ExceptionManager
{
    static ExceptionManager *mManager;
    static long __stdcall RunExceptionHandler (_EXCEPTION_POINTERS *ep);

    ExceptionManager (){};
    std::vector<std::function<void(_EXCEPTION_POINTERS *ep)>>
        mExceptionHandlers;

    static void DestroyExceptionManager ();

public:
    /// Registers a new Exception Handler. These are executed in order in a case
    /// of a crash
    void RegisterHandler (std::function<void(_EXCEPTION_POINTERS *ep)> handler);

    /// Initiates the Exception Manager
    void RegisterExceptionManager ();

    /// Returns/Creates the Exception Manager
    static ExceptionManager *GetExceptionManager ();
};

/*******************************************************/
template <unsigned int address, typename... Args>
void
Call (Args... args)
{
    reinterpret_cast<void(__cdecl *) (Args...)> (address) (args...);
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
    reinterpret_cast<void(__thiscall *) (C, Args...)> (address) (_this,
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
    reinterpret_cast<void(__thiscall *) (C, Args...)> (
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
