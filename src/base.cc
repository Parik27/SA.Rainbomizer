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
#include <algorithm>

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
RegisterHooks (std::vector<HookProperties> hooks)
{
    for (auto hook : hooks)
        {
            switch (hook.type)
                {
                case HOOK_JUMP:
                    injector::MakeJMP (hook.src, hook.dest);
                    Logger::GetLogger ()->LogMessage (
                        string_format ("JUMP: Hooking %X => %X", hook.src,
                                       hook.dest));
                    break;

                case HOOK_CALL:
                    injector::MakeCALL (hook.src, hook.dest);
                    Logger::GetLogger ()->LogMessage (
                        string_format ("CALL: Hooking %X => %X", hook.src,
                                       hook.dest));
                    break;
                }
        }
}

ExceptionManager *ExceptionManager::mManager = nullptr;

/*******************************************************/
void
ExceptionManager::RegisterHandler (
    std::function<void(_EXCEPTION_POINTERS *ep)> handler)
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
