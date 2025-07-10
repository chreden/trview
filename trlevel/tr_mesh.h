#pragma once

#include "trtypes.h"

#include <array>
#include <cstdint>
#include <SimpleMath.h>

namespace trlevel
{
    enum class BlendMode
    {
        None,
        Normal,
        Additive
    };

    struct trview_vertex
    {
        DirectX::SimpleMath::Vector3 vertex;
        int16_t lighting{ 0 };
        uint16_t attributes{ 0 };
        DirectX::SimpleMath::Color colour;
    };

    struct trview_mesh_face4
    {
        uint32_t                     vertices[4];
        std::array<DirectX::SimpleMath::Vector2, 4> uvs;
        DirectX::SimpleMath::Vector3 normals[4];
        uint32_t                     texture{ 0u };
        uint32_t                     effects{ 0u };
        BlendMode                    blend_mode{ BlendMode::None };
        bool                         double_sided{ false };
    };

    struct trview_mesh_face3
    {
        uint32_t                     vertices[3];
        DirectX::SimpleMath::Vector2 uvs[3];
        DirectX::SimpleMath::Vector3 normals[3];
        uint32_t                     texture{ 0u };
        uint32_t                     effects{ 0u };
        BlendMode                    blend_mode{ BlendMode::None };
        bool                         double_sided{ false };
    };

    std::vector<trview_mesh_face4> convert_rectangles_2(std::vector<tr_face4> rectangles);
    std::vector<trview_mesh_face4> convert_rectangles_2(std::vector<tr4_mesh_face4> rectangles);
    std::vector<trview_mesh_face3> convert_triangles_2(std::vector<tr_face3> triangles);
    std::vector<trview_mesh_face3> convert_triangles_2(std::vector<tr4_mesh_face3> triangles);
    std::vector<trview_vertex> convert_vertices(std::vector<tr_room_vertex> vertices);
    std::vector<trview_vertex> convert_vertices(std::vector<tr2_room_vertex> vertices);
    std::vector<trview_vertex> convert_vertices(std::vector<tr3_room_vertex> vertices);
    std::vector<trview_vertex> convert_vertices(std::vector<tr5_room_vertex> vertices);
}
