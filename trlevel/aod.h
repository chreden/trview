#pragma once

#include <cstdint>

namespace trlevel
{
    namespace aod
    {
        struct Header
        {
            struct Section
            {
                uint32_t unknown;
                uint32_t start;
                uint32_t size;
            };

            uint32_t version;
            uint32_t unknown_1;
            Section  sections[170];
        };
        static_assert(sizeof(Header) == 2048);

        struct Section1_Header
        {
            uint32_t num_animations;
            uint32_t count_2;
            uint32_t count_3;
            uint32_t count_4;
            uint32_t unknowns[44];
        };
        static_assert(sizeof(Section1_Header) == 192);

        struct Section1_Unknown
        {
            uint32_t unknown_1;
            uint32_t unknown_2;
            uint32_t unknown_3;
            uint32_t unknown_4;
        };

        struct Section1_Unknown2
        {
            char name[64];
            uint8_t unknown[68];
        };
        static_assert(sizeof(Section1_Unknown2) == 132);

        
    }
}
