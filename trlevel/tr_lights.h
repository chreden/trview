#pragma once

#include "trtypes.h"
#include "LevelVersion.h"
#include <trview.common/Colour.h>

namespace trlevel
{
    /// <summary>
    /// A structure that can hold any light.
    /// </summary>
    struct tr_x_room_light
    {
        LevelVersion level_version;
        tr_room_light  tr1;
        tr2_room_light tr2;
        tr3_room_light tr3;
        tr4_room_light tr4;
        tr5_room_light tr5;
        tr5_fog_bulb tr5_fog;

        DirectX::SimpleMath::Vector3 position() const;
        trview::Colour colour() const;
        LightType type() const;
        int32_t intensity() const;
        int32_t fade() const;
        DirectX::SimpleMath::Vector3 direction() const;
        float in() const;
        float out() const;
        float rad_in() const;
        float rad_out() const;
        float range() const;
        float length() const;
        float cutoff() const;
        float radius() const;
        float density() const;
    };

    std::string to_string(LightType type);

    std::vector<tr_x_room_light> convert_lights(std::vector<tr_room_light> lights);
    std::vector<tr_x_room_light> convert_lights(std::vector<tr_room_light_psx> lights);
    std::vector<tr_x_room_light> convert_lights(std::vector<tr2_room_light> lights);
    std::vector<tr_x_room_light> convert_lights(std::vector<tr3_room_light> lights);
    std::vector<tr_x_room_light> convert_lights(std::vector<tr4_room_light> lights);
    std::vector<tr_x_room_light> convert_lights(std::vector<tr5_room_light> lights);
}