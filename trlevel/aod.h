#pragma once

#include <cstdint>
#include <SimpleMath.h>

namespace trlevel
{
    namespace aod
    {
        enum class SectionType : uint32_t
        {
            Animation  = 1127259533,
            Animation2 = 3444339134,
            Animation3 = 3045493218,
            Unknown    = 3045516138,
            Unknown2   = 2883510172
        };

        struct Header
        {
            struct Section
            {
                SectionType type;
                uint32_t    start;
                uint32_t    size;
            };

            float    version;
            uint32_t unknown_1;
            Section  sections[170];
        };
        static_assert(sizeof(Header) == 2048);

        struct Section1_Header
        {
            uint32_t num_animations;
            uint32_t count_2;
            uint32_t count_3;
            uint32_t num_something;
            uint32_t count_5;
            uint32_t count_6;
        };
        static_assert(sizeof(Section1_Header) == 24);

        struct Section1_Unknown
        {
            uint32_t unknown_1;
            uint32_t unknown_2;
            uint32_t num_entries;
            uint32_t start;
        };

        struct Section1_Unknown2
        {
            char name[64];
            uint8_t unknown[68];
        };
        static_assert(sizeof(Section1_Unknown2) == 132);

        struct UnknownVertex
        {
            DirectX::SimpleMath::Vector3 position;
            DirectX::SimpleMath::Vector3 normal;
            float unknown[58]; // Probably transforms, scaling.
        };
    }
}
