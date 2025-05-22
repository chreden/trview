#include "tr_mesh.h"
#include <ranges>

namespace trlevel
{
    using namespace DirectX::SimpleMath;

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

    std::vector<trview_mesh_face4> convert_rectangles_2(std::vector<tr_face4> rectangles)
    {
        return rectangles |
            std::views::transform([](auto&& r) -> trview_mesh_face4 {
            return
            {
                .vertices = { static_cast<uint32_t>(r.vertices[0]), static_cast<uint32_t>(r.vertices[1]), static_cast<uint32_t>(r.vertices[2]), static_cast<uint32_t>(r.vertices[3]) },
                .texture = static_cast<uint32_t>(r.texture)
            };
                }) | std::ranges::to<std::vector>();
    }

    std::vector<trview_mesh_face4> convert_rectangles_2(std::vector<tr4_mesh_face4> rectangles)
    {
        return rectangles |
            std::views::transform([](auto&& r) -> trview_mesh_face4 {
            return
            {
                .vertices = { static_cast<uint32_t>(r.vertices[0]), static_cast<uint32_t>(r.vertices[1]), static_cast<uint32_t>(r.vertices[2]), static_cast<uint32_t>(r.vertices[3]) },
                .texture = static_cast<uint32_t>(r.texture),
                .effects = static_cast<uint32_t>(r.effects)
            }; }) | std::ranges::to<std::vector>();
    }

    std::vector<trview_mesh_face3> convert_triangles_2(std::vector<tr_face3> triangles)
    {
        return triangles |
            std::views::transform([](auto&& r) -> trview_mesh_face3 {
            return
            {
                .vertices = { static_cast<uint32_t>(r.vertices[0]), static_cast<uint32_t>(r.vertices[1]), static_cast<uint32_t>(r.vertices[2]) },
                .texture = static_cast<uint32_t>(r.texture)
            };
                }) | std::ranges::to<std::vector>();
    }

    std::vector<trview_mesh_face3> convert_triangles_2(std::vector<tr4_mesh_face3> triangles)
    {
        return triangles |
            std::views::transform([](auto&& r) -> trview_mesh_face3 {
            return
            {
                .vertices = { static_cast<uint32_t>(r.vertices[0]), static_cast<uint32_t>(r.vertices[1]), static_cast<uint32_t>(r.vertices[2]) },
                .texture = static_cast<uint32_t>(r.texture),
                .effects = static_cast<uint32_t>(r.effects)
            }; }) | std::ranges::to<std::vector>();
    }


    std::vector<trview_vertex> convert_vertices(std::vector<tr_room_vertex> vertices)
    {
        return vertices | std::views::transform([](auto&& v) -> trview_vertex {
            return { .vertex = { static_cast<float>(v.vertex.x), static_cast<float>(v.vertex.y), static_cast<float>(v.vertex.z) }, .lighting = v.lighting, .colour = lighting_to_colour(v.lighting) };
            }) | std::ranges::to<std::vector>();
    }

    std::vector<trview_vertex> convert_vertices(std::vector<tr2_room_vertex> vertices)
    {
        return vertices | std::views::transform([](auto&& v) -> trview_vertex {
            return { .vertex = { static_cast<float>(v.vertex.x), static_cast<float>(v.vertex.y), static_cast<float>(v.vertex.z) }, .lighting = v.lighting2, .colour = lighting_to_colour(v.lighting2) };
            }) | std::ranges::to<std::vector>();
    }

    std::vector<trview_vertex> convert_vertices(std::vector<tr3_room_vertex> vertices)
    {
        return vertices | std::views::transform([](auto&& v) -> trview_vertex {
            return { .vertex = { static_cast<float>(v.vertex.x), static_cast<float>(v.vertex.y), static_cast<float>(v.vertex.z) }, .lighting = v.lighting, .colour = to_colour(v.colour) };
            }) | std::ranges::to<std::vector>();
    }

    std::vector<trview_vertex> convert_vertices(std::vector<tr5_room_vertex> vertices)
    {
        return vertices | std::views::transform([](auto&& v) -> trview_vertex {
            return { .vertex = { static_cast<float>(v.vertex.x), static_cast<float>(v.vertex.y), static_cast<float>(v.vertex.z) }, .colour = to_colour(v.colour) };
            }) | std::ranges::to<std::vector>();
    }
}
