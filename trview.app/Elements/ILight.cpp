#include "ILight.h"
#include "IRoom.h"

namespace trview
{
    using namespace trlevel;

    ILight::~ILight()
    {
    }

    bool has_colour(const ILight& light)
    {
        return light.level_version() >= LevelVersion::Tomb3 && !(light.type() == LightType::FogBulb && light.level_version() == LevelVersion::Tomb4);
    }

    bool has_direction(const ILight& light)
    {
        return light.type() == LightType::Sun || light.type() == LightType::Spot;
    }

    bool has_fade(const ILight& light)
    {
        return light.level_version() < LevelVersion::Tomb4 && light.type() != LightType::Sun;
    }

    bool has_intensity(const ILight& light)
    {
        return light.level_version() < LevelVersion::Tomb5 && light.type() != LightType::Sun;
    }

    bool has_position(const ILight& light)
    {
        return light.type() != LightType::Sun;
    }

    bool has_hotspot(const ILight& light)
    {
        return light.level_version() >= LevelVersion::Tomb4 && equals_any(light.type(), LightType::Spot, LightType::Point, LightType::Shadow);
    }

    bool has_falloff(const ILight& light)
    {
        return light.level_version() >= LevelVersion::Tomb4 && equals_any(light.type(), LightType::Point, LightType::Shadow);
    }

    bool has_falloff_angle(const ILight& light)
    {
        return light.level_version() >= LevelVersion::Tomb4 && light.type() == LightType::Spot;
    }

    bool has_length(const ILight& light)
    {
        return light.level_version() == LevelVersion::Tomb4 && light.type() == LightType::Spot;
    }
    
    bool has_cutoff(const ILight& light)
    {
        return light.level_version() == LevelVersion::Tomb4 && light.type() == LightType::Spot;
    }

    bool has_rad_in(const ILight& light)
    {
        return light.level_version() == LevelVersion::Tomb5 && light.type() == LightType::Spot;
    }

    bool has_rad_out(const ILight& light)
    {
        return light.level_version() == LevelVersion::Tomb5 && light.type() == LightType::Spot;
    }

    bool has_range(const ILight& light)
    {
        return light.level_version() == LevelVersion::Tomb5 && light.type() == LightType::Spot;
    }

    bool has_density(const ILight& light)
    {
        return light.type() == LightType::FogBulb;
    }

    bool has_radius(const ILight& light)
    {
        return light.type() == LightType::FogBulb;
    }

    float fade(const ILight& light)
    {
        return static_cast<float>(light.fade());
    }

    float intensity(const ILight& light)
    {
        return static_cast<float>(light.intensity());
    }

    float hotspot(const ILight& light)
    {
        switch (light.type())
        {
            case LightType::Spot:
            {
                return DirectX::XMConvertToDegrees(std::acosf(light.in()));
            }
            case LightType::Point:
            case LightType::Shadow:
            {
                return light.in();
            }
        }
        return 0;
    }

    float falloff(const ILight& light)
    {
        switch (light.type())
        {
            case LightType::Point:
            case LightType::Shadow:
            {
                return light.out();
            }
        }
        return 0;
    }

    float falloff_angle(const ILight& light)
    {
        switch (light.type())
        {
            case LightType::Spot:
            {
                return DirectX::XMConvertToDegrees(std::acosf(light.out()));
            }
        }
        return 0;
    }

    float length(const ILight& light)
    {
        return light.length();
    }

    float cutoff(const ILight& light)
    {
        return light.cutoff();
    }

    float rad_in(const ILight& light)
    {
        return DirectX::XMConvertToDegrees(light.rad_in() * 0.5f);
    }

    float rad_out(const ILight& light)
    {
        return DirectX::XMConvertToDegrees(light.rad_out() * 0.5f);
    }

    float range(const ILight& light)
    {
        return light.range();
    }

    float density(const ILight& light)
    {
        return light.density();
    }

    float radius(const ILight& light)
    {
        return light.radius();
    }

    uint32_t light_room(const std::shared_ptr<ILight>& light)
    {
        if (!light)
        {
            return 0u;
        }
        return light_room(*light);
    }

    uint32_t light_room(const ILight& light)
    {
        if (auto room = light.room().lock())
        {
            return room->number();
        }
        return 0u;
    }
}
