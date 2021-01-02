#include "LevelSignature.h"

namespace trview
{
    void to_json(nlohmann::json& json, const LevelSignature& signature)
    {
        json = nlohmann::json
        {
            {"version", signature.version},
            {"items", signature.item_ids} 
        };
    }
}
