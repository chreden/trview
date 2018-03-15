#include "LevelVersion.h"

namespace trlevel
{
    // Converts the level version number into a level version enumeration.
    // This will never return tomb5 as more information is required in order to 
    // make that determination.
    // version: The version to convert.
    // Returns: The level version.
    LevelVersion convert_level_version(uint32_t version)
    {
        switch (version)
        {
        case 0x20:
            return LevelVersion::Tomb1;
        case 0x2D:
            return LevelVersion::Tomb2;
        case 0xFF180038:
        case 0xFF080038:
        case 0xFF180034:
            return LevelVersion::Tomb3;
        case 0x00345254:
        case 0x63345254:
            return LevelVersion::Tomb4;
        }
        return LevelVersion::Unknown;
    }
}