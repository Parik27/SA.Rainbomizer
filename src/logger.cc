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

#include "logger.hh"
#include <cstdio>
#include <ctime>
#include "base.hh"

Logger *    Logger::mInstance = nullptr;
std::string Logger::mFileName = "";

#ifndef NDEBUG
#define RAINBOMIZER_BUILD "Debug Build: " __DATE__ " " __TIME__
#else
#define RAINBOMIZER_BUILD "Build v3.2"
#endif

/*******************************************************/
std::string
GetTimeNow ()
{
    time_t currentTime;
    char   str[256];

    time (&currentTime);

    auto tm = std::localtime (&currentTime);
    sprintf (str, "%02d-%02d-%04d", tm->tm_mday, tm->tm_mon + 1,
             1900 + tm->tm_year);

    return str;
}

/*******************************************************/
Logger::Logger ()
{

    mFile = OpenRainbomizerFile (GetTimeNow () + ".txt", "a+", "logs/");

    if (!mFile)
        return;

    // Print initial message/time
    fprintf (mFile, "===========================================\n");
    fprintf (mFile, "%d\n", (int) time (NULL));
    fprintf (mFile, "Rainbomizer Build: %s \n", RAINBOMIZER_BUILD);
    fprintf (mFile, "===========================================\n");
}

/*******************************************************/
Logger::~Logger ()
{
    if (mFile)
        fclose (mFile);
}

/*******************************************************/
void
Logger::DestroyLogger ()
{
    if (Logger::mInstance)
        delete Logger::mInstance;
}

/*******************************************************/
Logger *
Logger::GetLogger ()
{
    if (!Logger::mInstance)
        {
            Logger::mInstance = new Logger ();
            atexit (&Logger::DestroyLogger);
        }
    return Logger::mInstance;
}

/*******************************************************/
void
Logger::LogMessage (std::string message)
{
    if (!mFile)
        return;

    fprintf (mFile, "[%d]: %s\n", (int) time (NULL), message.c_str ());
    fflush (mFile);
}
