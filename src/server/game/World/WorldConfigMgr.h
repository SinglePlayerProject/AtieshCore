/*
 * Copyright (C) since 2016 AtieshCore <https://at-wow.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GAME_CONFIG_H
#define _GAME_CONFIG_H

#include "Common.h"
#include <unordered_map>

enum class WorldConfigType : uint8
{
    GAME_CONFIG_TYPE_BOOL,
    GAME_CONFIG_TYPE_INT,
    GAME_CONFIG_TYPE_FLOAT,
    GAME_CONFIG_TYPE_RATES,

    GAME_CONFIG_TYPE_UNKNOWN
};

class TC_GAME_API WorldConfig
{
public:
    static WorldConfig* instance();

    void Load();

    void RecheckAndFixDependancy();

private:
    // Add config options
    void AddOption(WorldConfigType type, uint32 IDinTypeGroup, std::string const& defaultValue, std::string const& value);

    void AddBoolOption(uint32 IDinTypeGroup, bool const& value);
    void AddIntOption(uint32 IDinTypeGroup, uint32 const& value);
    void AddFloatOption(uint32 IDinTypeGroup, float const& value);
    void AddRateOption(uint32 IDinTypeGroup, float const& value);
};

#define sWorldConfig WorldConfig::instance()

#endif // _GAME_CONFIG_H
