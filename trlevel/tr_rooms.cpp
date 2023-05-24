#include "stdafx.h"
#include "tr_rooms.h"

using namespace DirectX::SimpleMath;

namespace trlevel
{
    namespace
    {
        /// <summary>
        /// Convert TR1/2 brightness to Color.
        /// </summary>
        constexpr Color lighting_to_colour(int16_t lighting) noexcept
        {
            const float value = 1.0f - static_cast<float>(lighting) / 8191.0f;
            return Color(value, value, value);
        }

        /// <summary>
        /// Convert TR3/4 colour to Color. Doubles values to look like PSX.
        /// </summary>
        constexpr Color to_colour(uint16_t colour) noexcept
        {
            const int32_t r = (colour & 0x7c00) >> 10;
            const int32_t g = (colour & 0x03E0) >> 5;
            const int32_t b = (colour & 0x001F);
            return Color(r / 16.5f, g / 16.5f, b / 16.5f);
        }

        /// <summary>
        /// Convert TR5 colour to Color. Doubles values to look like PSX.
        /// </summary>
        constexpr Color to_colour(uint32_t colour) noexcept
        {
            const int32_t r = (colour & 0x00ff0000) >> 16;
            const int32_t g = (colour & 0x0000ff00) >> 8;
            const int32_t b = (colour & 0x000000ff);
            return Color(r / 128.0f, g / 128.0f, b / 128.0f);
        }
    }

    std::vector<trview_room_vertex> convert_vertices(std::vector<tr_room_vertex> vertices)
    {
        std::vector<trview_room_vertex> new_vertices;
        new_vertices.reserve(vertices.size());
        std::transform(vertices.begin(), vertices.end(),
            std::back_inserter(new_vertices), [](const auto& vert)
            {
                trview_room_vertex new_vertex{ vert.vertex, vert.lighting, 0, lighting_to_colour(vert.lighting) };
                return new_vertex;
            });
        return new_vertices;
    }

    std::vector<trview_room_vertex> convert_vertices(std::vector<tr2_room_vertex> vertices)
    {
        std::vector<trview_room_vertex> new_vertices;
        new_vertices.reserve(vertices.size());
        std::transform(vertices.begin(), vertices.end(),
            std::back_inserter(new_vertices), [](const auto& vert)
            {
                trview_room_vertex new_vertex{ vert.vertex, vert.lighting2, 0, lighting_to_colour(vert.lighting2) };
                return new_vertex;
            });
        return new_vertices;
    }

    std::vector<trview_room_vertex> convert_vertices(std::vector<tr3_room_vertex> vertices)
    {
        std::vector<trview_room_vertex> new_vertices;
        new_vertices.reserve(vertices.size());
        std::transform(vertices.begin(), vertices.end(),
            std::back_inserter(new_vertices), [](const auto& vert)
            {
                trview_room_vertex new_vertex{ vert.vertex, vert.lighting, vert.attributes, to_colour(vert.colour) };
                return new_vertex;
            });
        return new_vertices;
    }

    std::vector<trview_room_vertex> convert_vertices(std::vector<tr5_room_vertex> vertices)
    {
        std::vector<trview_room_vertex> new_vertices;
        new_vertices.reserve(vertices.size());
        std::transform(vertices.begin(), vertices.end(),
            std::back_inserter(new_vertices), [](const auto& vert)
            {
                tr_vertex vertex{ static_cast<int16_t>(vert.vertex.x), static_cast<int16_t>(vert.vertex.y), static_cast<int16_t>(vert.vertex.z) };
                return trview_room_vertex{ vertex, 0, 0, to_colour(vert.colour) };
            });
        return new_vertices;
    }
}