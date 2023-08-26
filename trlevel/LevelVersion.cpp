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
        switch (version)
        {
        case 0x20:
            return { .platform = Platform::PC, .version = LevelVersion::Tomb1 };
        case 0xB020:
            return { .platform = Platform::PSX, .version = LevelVersion::Tomb1 };
        case 0x2D:
            return { .platform = Platform::PC, .version = LevelVersion::Tomb2 };
        case 0xFF180038:
        case 0xFF080038:
        case 0xFF180034:
            return { .platform = Platform::PC, .version = LevelVersion::Tomb3 }; 
        case 0xffffffc8:
            return { .platform = Platform::PSX, .version = LevelVersion::Tomb3 };
        case 0x00345254:
        case 0x63345254:
            return { .platform = Platform::PC, .version = LevelVersion::Tomb4 };
        }
        return { .platform = Platform::Unknown, .version = LevelVersion::Unknown };
    }
}