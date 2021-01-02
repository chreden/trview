#pragma once

#include <cstdint>
#include <vector>
#include <trview.app/Elements/Level.h>
#include <external/nlohmann/json.hpp>

namespace trview
{
    struct LevelSignature
    {
        trlevel::LevelVersion version;
        std::vector<uint32_t> item_ids;
        
    };

    void to_json(nlohmann::json& json, const LevelSignature& signature);
}
