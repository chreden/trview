#include "LevelVersion.h"
#include <trview.common/Algorithms.h>
#include "Level_psx.h"

namespace trlevel
{
    namespace
    {
        PlatformAndVersion platform_and_version(int32_t version)
        {
            if (version == 0x34585254)
            {
                return { .platform = Platform::PC, .version = LevelVersion::Tomb4, .remastered = true };
            }
            else if (version == 0x35585254)
            {
                return { .platform = Platform::PC, .version = LevelVersion::Tomb5, .remastered = true };
            }
            else if (version == 0x00000000)
            {
                return { .platform = Platform::PSX, .is_pack = true };
            }
            else if (version == 0x6a20 || version == 0x6ba8)
            {
                return { .platform = Platform::PSX, .version = LevelVersion::Tomb4 };
            }
            else if (is_supported_tr4_psx_version(version))
            {
                return { .platform = Platform::PSX, .version = LevelVersion::Tomb4 };
            }
            else if (is_supported_tr5_psx_version(version))
            {
                return { .platform = Platform::PSX, .version = LevelVersion::Tomb5 };
            }

            switch (version & 0xff)
            {
            case 0x09:
            case 0x0B:
            case 0x15:
            case 0x20:
                return { .platform = (version & 0xff00) ? Platform::PSX : Platform::PC, .version = LevelVersion::Tomb1 };
            case 0x26:
            case 0x2D:
                return { .platform = Platform::PC, .version = LevelVersion::Tomb2 };
            case 0x34:
            case 0x38:
                return { .platform = Platform::PC, .version = LevelVersion::Tomb3 };
            case 0xc8:
            case 0xc9:
            case 0xcb:  //TR3 PSX ECTS Demos
                return { .platform = Platform::PSX, .version = LevelVersion::Tomb3 };
            case 0x54:
                return { .platform = Platform::PC, .version = LevelVersion::Tomb4 };
            case 0x88:
            case 0xfc:
                return { .platform = Platform::PSX, .version = LevelVersion::Tomb4 };
            case 0x3c:
            case 0xd8:
                return { .platform = Platform::Dreamcast, .version = LevelVersion::Tomb5 };
            }
            return { .platform = Platform::Unknown, .version = LevelVersion::Unknown };
        }
    }

    // Converts the level version number into a level version enumeration.
    // This will never return tomb5 as more information is required in order to 
    // make that determination.
    // version: The version to convert.
    // Returns: The level version.
    PlatformAndVersion convert_level_version(int32_t version)
    {
        auto result = platform_and_version(version);
        result.raw_version = version;
        return result;
    }

    bool is_tr1_version_21(PlatformAndVersion version)
    {
        return version.raw_version == 21;
    }

    bool is_tr1_may_1996(PlatformAndVersion version)
    {
        return version.raw_version == 11 || version.raw_version == 9;
    }

    bool is_tr1_pc_may_1996(PlatformAndVersion version)
    {
        return is_tr1_may_1996(version) && version.platform == Platform::PC;
    }

    bool is_tr1_version_26(PlatformAndVersion version)
    {
        return version.raw_version == 26;
    }

    bool is_tr1_version_27(PlatformAndVersion version)
    {
        return version.raw_version == 27;
    }

    bool is_tr2_version_38(PlatformAndVersion version)
    {
        return version.raw_version == 38;
    }

    bool is_tr2_version_42(PlatformAndVersion version)
    {
        return version.raw_version == 42;
    }

    bool is_tr2_version_44(PlatformAndVersion version)
    {
        return version.raw_version == 44;
    }

    bool is_tr2_e3(PlatformAndVersion version)
    {
        return version.raw_version == 38;
    }

    bool is_tr3_ects(PlatformAndVersion version)
    {
        return version.raw_version == -53;
    }

    bool is_tr3_demo_55(PlatformAndVersion version)
    {
        return version.raw_version == -55;
    }

    bool is_tr4_version_111(PlatformAndVersion version)
    {
        return version.raw_version == -111;
    }

    bool is_tr4_version_121(PlatformAndVersion version)
    {
        return version.raw_version == -121;
    }

    bool is_tr4_version_126(PlatformAndVersion version)
    {
        return version.raw_version == -126;
    }

    bool is_tr4_oct_1999(PlatformAndVersion version)
    {
        return version.raw_version == 0x6a20;
    }

    bool is_tr4_opsm_90(PlatformAndVersion version)
    {
        return version.raw_version == -120;
    }

    bool is_tr4_psx_v1(PlatformAndVersion version)
    {
        return version.raw_version == 0x6ba8;
    }

    bool has_double_frames(PlatformAndVersion version)
    {
        return is_tr1_may_1996(version) || is_tr1_version_21(version);
    }
}