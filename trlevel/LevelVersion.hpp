#pragma once

namespace trlevel
{
    constexpr std::string to_string(LevelVersion version) noexcept
    {
        switch (version)
        {
        case LevelVersion::Tomb1:
            return "Tomb1";
        case LevelVersion::Tomb2:
            return "Tomb2";
        case LevelVersion::Tomb3:
            return "Tomb3";
        case LevelVersion::Tomb4:
            return "Tomb4";
        case LevelVersion::Tomb5:
            return "Tomb5";
        }
        return "Unknown";
    }

    constexpr std::string to_string(Platform platform) noexcept
    {
        switch (platform)
        {
        case Platform::PC:
            return "PC";
        case Platform::PSX:
            return "PSX";
        case Platform::Dreamcast:
            return "Dreamcast";
        case Platform::Saturn:
            return "Saturn";
        }
        return "Unknown";
    }
}
