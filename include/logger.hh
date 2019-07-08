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

#include <string>

class Logger
{
    FILE *             mFile;
    static std::string mFileName;
    static Logger *    mInstance;

    Logger ();
    ~Logger ();
    static void DestroyLogger ();

public:
    /// Returns the static instance for Logger.
    static Logger *GetLogger ();

    /// Sets the file name for the logger (only if it's not already created)
    void
    SetFileName (std::string name)
    {
        mFileName = name;
    }

    /// Gets the current file name for the logger
    std::string
    GetFileName ()
    {
        return mFileName;
    }

    /// Logs a message to the logger
    void LogMessage (std::string message);
};
