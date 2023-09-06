#pragma once

namespace trlevel
{
    constexpr LightType convert(LightTypeTR3 type)
    {
        return type == LightTypeTR3::Point ? LightType::Point : LightType::Sun;
    }
}
