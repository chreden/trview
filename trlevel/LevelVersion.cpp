#include "LevelVersion.h"

namespace trlevel
{
    // Converts the level version number into a level version enumeration.
    // This will never return tomb5 as more information is required in order to 
    // make that determination.
    // version: The version to convert.
    // Returns: The level version.
    PlatformAndVersion convert_level_version(uint32_t version)
    {
        if (version == 0x4D4F4F52)
        {
            // Note - this is not a version number, it's just ROOM
            return { .platform = Platform::Saturn, .version = LevelVersion::Tomb1 };
        }

        switch (version & 0xff)
        {
        case 0x20:
            return { .platform = (version & 0xff00) ? Platform::PSX : Platform::PC, .version = LevelVersion::Tomb1 };
        case 0x2D:
            return { .platform = Platform::PC, .version = LevelVersion::Tomb2 };
        case 0x34:
        case 0x38:
            return { .platform = Platform::PC, .version = LevelVersion::Tomb3 }; 
        case 0xc8:
            return { .platform = Platform::PSX, .version = LevelVersion::Tomb3 };
        case 0x54:
            return { .platform = Platform::PC, .version = LevelVersion::Tomb4 };
        }
        return { .platform = Platform::Unknown, .version = LevelVersion::Unknown };
    }
}