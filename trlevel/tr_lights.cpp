#include "tr_lights.h"

namespace trlevel
{
    using namespace trview;
    using namespace DirectX::SimpleMath;

    Vector3 tr_x_room_light::position() const
    {
        switch (level_version)
        {
        case LevelVersion::Tomb1:
            return Vector3(static_cast<float>(tr1.x), static_cast<float>(tr1.y), static_cast<float>(tr1.z)) / Scale_X;
        case LevelVersion::Tomb2:
            return Vector3(static_cast<float>(tr2.x), static_cast<float>(tr2.y), static_cast<float>(tr2.z)) / Scale_X;
        case LevelVersion::Tomb3:
            return tr3.position();
        case LevelVersion::Tomb4:
            return Vector3(static_cast<float>(tr4.x), static_cast<float>(tr4.y), static_cast<float>(tr4.z)) / Scale_X;
        case LevelVersion::Tomb5:
            return type() == LightType::FogBulb ? 
                Vector3(tr5_fog.position.x, tr5_fog.position.y, tr5_fog.position.z) / Scale_X :
                Vector3(tr5.position.x, tr5.position.y, tr5.position.z) / Scale_X;
        }
        return Vector3::Zero;
    }

    Colour tr_x_room_light::colour() const
    {
        switch (level_version)
        {
        case LevelVersion::Tomb1:
        case LevelVersion::Tomb2:
            return Colour::White;
        case LevelVersion::Tomb3:
            return Colour(tr3.colour.Red / 255.0f, tr3.colour.Green / 255.0f, tr3.colour.Blue / 255.0f);
        case LevelVersion::Tomb4:
            return Colour(tr4.colour.Red / 255.f, tr4.colour.Green / 255.0f, tr4.colour.Blue / 255.0f);
        case LevelVersion::Tomb5:
            return type() == LightType::FogBulb ? 
                Colour(tr5_fog.colour.r, tr5_fog.colour.g, tr5_fog.colour.b) :
                Colour(tr5.colour.r, tr5.colour.g, tr5.colour.b);
        }
        return Colour::Transparent;
    }

    LightType tr_x_room_light::type() const
    {
        switch (level_version)
        {
        case LevelVersion::Tomb1:
        case LevelVersion::Tomb2:
            return LightType::Point;
        case LevelVersion::Tomb3:
            return tr3.type;
        case LevelVersion::Tomb4:
            return tr4.light_type;
        case LevelVersion::Tomb5:
            return tr5.light_type;
        }
        return LightType::Point;
    }

    int32_t tr_x_room_light::intensity() const
    {
        switch (level_version)
        {
        case LevelVersion::Tomb1:
            return tr1.intensity;
        case LevelVersion::Tomb2:
            return tr2.intensity1;
        case LevelVersion::Tomb3:
            return tr3.point.intensity;
        case LevelVersion::Tomb4:
            return tr4.intensity;
        case LevelVersion::Tomb5:
            return 0;
        }
        return 0;
    }

    int32_t tr_x_room_light::fade() const
    {
        switch (level_version)
        {
        case LevelVersion::Tomb1:
            return tr1.fade;
        case LevelVersion::Tomb2:
            return tr2.fade1;
        case LevelVersion::Tomb3:
            return tr3.point.fade;
        case LevelVersion::Tomb4:
        case LevelVersion::Tomb5:
            return 0;
        }
        return 0;
    }

    Vector3 tr_x_room_light::direction() const
    {
        switch (level_version)
        {
            case LevelVersion::Tomb3:
            {
                if (type() == LightType::Sun)
                {
                    return Vector3(tr3.sun.nx, tr3.sun.ny, tr3.sun.nz) / Scale_X;
                }
                return Vector3::Zero;
            }
            case LevelVersion::Tomb4:
            {
                if (type() == LightType::Sun || type() == LightType::Spot)
                {
                    return Vector3(tr4.dx, tr4.dy, tr4.dz) / Scale_X;
                }
                return Vector3::Zero;
            }
            case LevelVersion::Tomb5:
            {
                if (type() == LightType::Sun || type() == LightType::Spot)
                {
                    return Vector3(tr5.direction.x, tr5.direction.y, tr5.direction.z) / Scale_X;
                }
                return Vector3::Zero;
            }
        }

        return Vector3::Zero;
    }

    float tr_x_room_light::in() const
    {
        switch (level_version)
        {
            case LevelVersion::Tomb4:
            {
                return tr4.in;
            }
            case LevelVersion::Tomb5:
            {
                return tr5.in;
            }
        }
        return 0;
    }

    float tr_x_room_light::out() const
    {
        switch (level_version)
        {
            case LevelVersion::Tomb4:
            {
                return tr4.out;
            }
            case LevelVersion::Tomb5:
            {
                return tr5.out;
            }
        }
        return 0;
    }

    float tr_x_room_light::rad_in() const
    {
        switch (level_version)
        {
            case LevelVersion::Tomb5:
            {
                return tr5.rad_in;
            }
        }
        return 0;
    }

    float tr_x_room_light::rad_out() const
    {
        switch (level_version)
        {
            case LevelVersion::Tomb5:
            {
                return tr5.rad_out;
            }
        }
        return 0;
    }

    float tr_x_room_light::range() const
    {
        switch (level_version)
        {
            case LevelVersion::Tomb5:
            {
                return tr5.range;
            }
        }
        return 0;
    }

    float tr_x_room_light::length() const
    {
        switch (level_version)
        {
            case LevelVersion::Tomb4:
            {
                return tr4.length;
            }
        }
        return 0;
    }
    
    float tr_x_room_light::cutoff() const
    {
        switch (level_version)
        {
            case LevelVersion::Tomb4:
            {
                return tr4.cutoff;
            }
        }
        return 0;
    }

    float tr_x_room_light::radius() const
    {
        if (type() == LightType::FogBulb)
        {
            switch (level_version)
            {
                case LevelVersion::Tomb4:
                {
                    return tr4.out;
                }
                case LevelVersion::Tomb5:
                {
                    return tr5_fog.radius;
                }
            }
        }
        return 0;
    }

    float tr_x_room_light::density() const
    {
        if (type() == LightType::FogBulb)
        {
            switch (level_version)
            {
                case LevelVersion::Tomb4:
                {
                    return static_cast<float>(tr4.colour.Red) / 255.0f;
                }
                case LevelVersion::Tomb5:
                {
                    return tr5_fog.density;
                }
            }
        }
        return 0;
    }

    std::string light_type_name(LightType type)
    {
        switch (type)
        {
        case LightType::Sun:
            return "Sun";
        case LightType::Point:
            return "Point";
        case LightType::Spot:
            return "Spot";
        case LightType::Shadow:
            return "Shadow";
        case LightType::FogBulb:
            return "Fog Bulb";
        }
        return "Unknown";
    }

    std::vector<tr_x_room_light> convert_lights(std::vector<tr_room_light> lights)
    {
        std::vector<tr_x_room_light> new_lights;
        new_lights.reserve(lights.size());
        std::transform(lights.begin(), lights.end(),
            std::back_inserter(new_lights), [](const auto& light)
            {
                tr_x_room_light new_light;
                new_light.level_version = LevelVersion::Tomb1;
                new_light.tr1 = light;
                return new_light;
            });
        return new_lights;
    }

    std::vector<tr_x_room_light> convert_lights(std::vector<tr2_room_light> lights)
    {
        std::vector<tr_x_room_light> new_lights;
        new_lights.reserve(lights.size());
        std::transform(lights.begin(), lights.end(),
            std::back_inserter(new_lights), [](const auto& light)
            {
                tr_x_room_light new_light;
                new_light.level_version = LevelVersion::Tomb2;
                new_light.tr2 = light;
                return new_light;
            });
        return new_lights;
    }

    std::vector<tr_x_room_light> convert_lights(std::vector<tr3_room_light> lights)
    {
        std::vector<tr_x_room_light> new_lights;
        new_lights.reserve(lights.size());
        std::transform(lights.begin(), lights.end(),
            std::back_inserter(new_lights), [](const auto& light)
            {
                tr_x_room_light new_light;
                new_light.level_version = LevelVersion::Tomb3;
                new_light.tr3 = light;
                return new_light;
            });
        return new_lights;
    }

    std::vector<tr_x_room_light> convert_lights(std::vector<tr4_room_light> lights)
    {
        std::vector<tr_x_room_light> new_lights;
        new_lights.reserve(lights.size());
        std::transform(lights.begin(), lights.end(),
            std::back_inserter(new_lights), [](const auto& light)
            {
                tr_x_room_light new_light;
                new_light.level_version = LevelVersion::Tomb4;
                new_light.tr4 = light;
                return new_light;
            });
        return new_lights;
    }

    std::vector<tr_x_room_light> convert_lights(std::vector<tr5_room_light> lights)
    {
        std::vector<tr_x_room_light> new_lights;
        new_lights.reserve(lights.size());
        std::transform(lights.begin(), lights.end(),
            std::back_inserter(new_lights), [](const auto& light)
            {
                tr_x_room_light new_light;
                new_light.level_version = LevelVersion::Tomb5;
                new_light.tr5 = light;
                return new_light;
            });
        return new_lights;
    }
}
