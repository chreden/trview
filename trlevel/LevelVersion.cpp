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
            else if (version == 0x00000000)
            {
                return { .platform = Platform::PSX, .is_pack = true };
            }
            else if (version == 0x6a20 || version == 0x6ba8)
            {
                return { .platform = Platform::PSX, .version = LevelVersion::Tomb4 };
            }
            else if (version == 0x4D4F4F52)
            {
                return { .platform = Platform::Saturn, .version = LevelVersion::Tomb1 };
            }

            switch (version & 0xff)
            {
            case 0x09:
            case 0x0B:
            case 0x20:
                if (is_tr2_beta(version))
                {
                    return { .platform = Platform::PSX, .version = LevelVersion::Tomb2 };
                }
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
    PlatformAndVersion convert_level_version(uint32_t version)
    {
        auto result = platform_and_version(version);
        result.raw_version = version;
        return result;
    }

    bool is_tr1_may_1996(PlatformAndVersion version)
    {
        return version.raw_version == 11 || version.raw_version == 9;
    }

    bool is_tr1_pc_may_1996(PlatformAndVersion version)
    {
        return is_tr1_may_1996(version) && version.platform == Platform::PC;
    }

    bool is_tr1_aug_1996(PlatformAndVersion version)
    {
        return version.raw_version == 27;
    }

    bool is_tr2_beta(uint32_t version)
    {
        return ((version & 0xff) == 0x20) && (version & 0xff0000 || trview::equals_any(version, 0xf820u, 0xd620u, 0x1220u, 0x1a20u));
    }

    bool is_tr2_beta(PlatformAndVersion version)
    {
        return is_tr2_beta(version.raw_version);
    }

    bool is_tr2_demo_70688(PlatformAndVersion version)
    {
        return version.raw_version == 70688;
    }

    bool is_tr2_e3(PlatformAndVersion version)
    {
        return version.raw_version == 0x26;
    }

    bool is_tr3_ects(PlatformAndVersion version)
    {
        return version.raw_version == -53;
    }

    bool is_tr3_demo_55(PlatformAndVersion version)
    {
        return version.raw_version == -55;
    }

    bool is_tr4_oct_1999(PlatformAndVersion version)
    {
        return version.raw_version == 0x6a20;
    }

    bool is_tr4_opsm_90(PlatformAndVersion version)
    {
        return version.raw_version == 0xffffff88;
    }

    bool is_tr4_psx_v1(PlatformAndVersion version)
    {
        return version.raw_version == 0x6ba8;
    }
}