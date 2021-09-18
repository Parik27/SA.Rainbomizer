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
#include <memory>
#include <vector>
#include <cstdio>
#include <functional>
#include <cstdint>
#include <utility>
#include <optional>

namespace cpptoml {
class table;
} // namespace cpptoml

/*******************************************************/
class ConfigManager
{
    std::shared_ptr<cpptoml::table> m_pDefaultConfig;
    std::shared_ptr<cpptoml::table> m_pConfig;

    ConfigManager (){};

    void WriteDefaultConfig (const std::string &file);
    std::shared_ptr<cpptoml::table> ParseDefaultConfig ();

    template <typename T>
    void ReadValue (const std::string &tableName, const std::string &key,
                    T &out);

    bool GetIsEnabled (const std::string &name);

public:
    /// Returns the static instance for ConfigManager.
    static ConfigManager *GetInstance ();

    /// Initialises
    ConfigManager (const std::string &file = "config.toml");

    template <typename... Args>
    static bool
    ReadConfig (const std::string &table, Args... params)
    {
        if (!GetInstance ()->GetIsEnabled (table))
            return false;

        (GetInstance ()->ReadValue (table, params.first, *params.second), ...);
        return true;
    }
};
