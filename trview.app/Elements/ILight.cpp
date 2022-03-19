#include "ILight.h"

namespace trview
{
    ILight::~ILight()
    {
    }

    bool has_colour(const ILight& light)
    {
        return !(light.level_version() < trlevel::LevelVersion::Tomb3 || light.type() == trlevel::LightType::FogBulb && light.level_version() == trlevel::LevelVersion::Tomb4);
    }

    bool has_direction(const ILight& light)
    {
        return light.type() == trlevel::LightType::Sun || light.type() == trlevel::LightType::Spot;
    }
}
