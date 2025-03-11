#include "LevelVersion.h"
#include <trview.common/Algorithms.h>

namespace trlevel
{
    namespace
    {
        PlatformAndVersion platform_and_version(uint32_t version)
        {
            if (version == 0x34585254)
            {
                return { .platform = Platform::PC, .version = LevelVersion::Tomb4, .remastered = true };
            }
            else if (version == 0x35585254)
            {
                return { .platform = Platform::PC, .version = LevelVersion::Tomb5, .remastered = true };
            }

            switch (version & 0xff)
            {
            case 0x20:
                if (is_tr2_beta(version))
                {
                    return { .platform = Platform::PSX, .version = LevelVersion::Tomb2 };
                }
                return { .platform = (version & 0xff00) ? Platform::PSX : Platform::PC, .version = LevelVersion::Tomb1 };
            case 0x2D:
                return { .platform = Platform::PC, .version = LevelVersion::Tomb2 };
            case 0x34:
            case 0x38:
                return { .platform = Platform::PC, .version = LevelVersion::Tomb3 };
            case 0xc8:
            case 0xcb:  //TR3 PSX ECTS Demos
                return { .platform = Platform::PSX, .version = LevelVersion::Tomb3 };
            case 0x54:
                return { .platform = Platform::PC, .version = LevelVersion::Tomb4 };
            }
            return { .platform = Platform::Unknown, .version = LevelVersion::Unknown };
        }
    }

    // Converts the level version number into a level version enumeration.
    // This will never return tomb5 as more information is required in order to 
    // make that determination.
    // version: The version to convert.
    // Returns: The level version.
    PlatformAndVersion convert_level_version(uint32_t version)
    {
        auto result = platform_and_version(version);
        result.raw_version = version;
        return result;
    }

    constexpr bool is_tr2_beta(uint32_t version)
    {
        return ((version & 0xff) == 0x20) && (version & 0xff0000 || trview::equals_any(version, 0xf820u, 0xd620u, 0x1220u, 0x1a20u));
    }
}