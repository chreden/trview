#pragma once

#include <cstdint>
#include <vector>
#include <trlevel/LevelVersion.h>

namespace trview
{
    struct LevelSignature
    {
        trlevel::LevelVersion version;
        std::vector<uint32_t> item_ids;
    };
}
