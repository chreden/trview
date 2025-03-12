#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

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

    enum class Platform
    {
        Unknown,
        PC,
        PSX
    };

    struct PlatformAndVersion final
    {
        Platform platform{ Platform::Unknown };
        LevelVersion version{ LevelVersion::Unknown };
        bool remastered{ false };
        uint32_t raw_version{ 0 };
    };

    inline bool operator==(const PlatformAndVersion & l, const PlatformAndVersion & r)
    {
        return l.platform == r.platform && l.version == r.version && l.remastered == r.remastered;
    }

    // Converts the level version number into a level version enumeration.
    // This will never return tomb5 as more information is required in order to 
    // make that determination.
    // version: The version to convert.
    // Returns: The level version.
    PlatformAndVersion convert_level_version(uint32_t version);

    bool is_tr2_beta(uint32_t version);
    bool is_tr2_beta(PlatformAndVersion version);
    bool is_tr2_demo_70688(PlatformAndVersion version);
    bool is_tr3_ects(PlatformAndVersion version);
    bool is_tr3_demo_55(PlatformAndVersion version);

    constexpr std::string to_string(LevelVersion version) noexcept;
    constexpr std::string to_string(Platform platform) noexcept;
}

template <>
struct std::hash<trlevel::PlatformAndVersion>
{
    std::size_t operator()(const trlevel::PlatformAndVersion& p) const
    {
        std::size_t result = 17;
        result = result * 31 + std::hash<trlevel::Platform>()(p.platform);
        result = result * 31 + std::hash<trlevel::LevelVersion>()(p.version);
        result = result * 31 + std::hash<bool>()(p.remastered);
        return result;
    }
};

#include "LevelVersion.hpp"
