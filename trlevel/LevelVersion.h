#pragma once

#include <cstdint>
#include <string>

namespace trlevel
{
    enum class LevelVersion
    {
        Unknown,
        Tomb1,
        Tomb2,
        Tomb3,
        Tomb4,
        Tomb5
    };

    // Converts the level version number into a level version enumeration.
    // This will never return tomb5 as more information is required in order to 
    // make that determination.
    // version: The version to convert.
    // Returns: The level version.
    LevelVersion convert_level_version(uint32_t version);

    constexpr std::string to_string(LevelVersion version) noexcept;
}

#include "LevelVersion.hpp"
