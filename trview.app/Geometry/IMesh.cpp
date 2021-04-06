#include "IMesh.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        const uint16_t Texture_Mask_TR4 = 0x7fff;
        const uint16_t Texture_Mask = 0x0fff;

        uint16_t texture_mask(trlevel::LevelVersion level_version)
        {
            return level_version == trlevel::LevelVersion::Tomb4 ? Texture_Mask_TR4 : Texture_Mask;
        }

        Vector3 calculate_normal(const Vector3* const vertices)
        {
            auto first = vertices[1] - vertices[0];
            auto second = vertices[2] - vertices[0];
            first.Normalize();
            second.Normalize();
            return first.Cross(second);
        }
    }

    IMesh::~IMesh()
    {
    }

    std::unique_ptr<IMesh> create_mesh(trlevel::LevelVersion level_version, const trlevel::tr_mesh& mesh, const IMesh::Source& source, const ILevelTextureStorage& texture_storage, bool transparent_collision)
    {
        std::vector<std::vector<uint32_t>> indices(texture_storage.num_tiles());
        std::vector<MeshVertex> vertices;
        std::vector<uint32_t> untextured_indices;
        std::vector<TransparentTriangle> transparent_triangles;
        std::vector<Triangle> collision_triangles;

        process_textured_rectangles(level_version, mesh.textured_rectangles, mesh.vertices, texture_storage, vertices, indices, transparent_triangles, collision_triangles, transparent_collision);
        process_textured_triangles(level_version, mesh.textured_triangles, mesh.vertices, texture_storage, vertices, indices, transparent_triangles, collision_triangles, transparent_collision);
        process_coloured_rectangles(mesh.coloured_rectangles, mesh.vertices, texture_storage, vertices, untextured_indices, collision_triangles);
        process_coloured_triangles(mesh.coloured_triangles, mesh.vertices, texture_storage, vertices, untextured_indices, collision_triangles);

        return source(vertices, indices, untextured_indices, transparent_triangles, collision_triangles);
    }

    std::unique_ptr<IMesh> create_cube_mesh(const IMesh::Source& source)
    {
        const std::vector<MeshVertex> vertices
        {
            // + y
            { { -0.5, 0.5f, -0.5 }, Vector3::Down, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },       // 2
            { { 0.5, 0.5f, -0.5 }, Vector3::Down, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },        // 1
            { { 0.5, 0.5f, 0.5 }, Vector3::Down, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },         // 0
            { { -0.5, 0.5f, 0.5 }, Vector3::Down, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },        // 3

            // +x
            { { 0.5, -0.5f, -0.5 }, Vector3::Left, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },       // 5
            { { 0.5, -0.5f, 0.5 }, Vector3::Left, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },        // 4
            { { 0.5, 0.5f, 0.5 }, Vector3::Left, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },         // 0
            { { 0.5, 0.5f, -0.5 }, Vector3::Left, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },        // 1

            // -x 
            { { -0.5, 0.5f, -0.5 }, Vector3::Right, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },       // 2
            { { -0.5, 0.5f, 0.5 }, Vector3::Right, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },        // 3
            { { -0.5, -0.5f, 0.5 }, Vector3::Right, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },       // 7
            { { -0.5, -0.5f, -0.5 }, Vector3::Right, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },      // 6

            // +z
            { { 0.5, 0.5f, 0.5 }, Vector3::Forward, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },         // 0
            { { 0.5, -0.5f, 0.5 }, Vector3::Forward, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },        // 4
            { { -0.5, 0.5f, 0.5 }, Vector3::Forward, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },        // 3
            { { -0.5, -0.5f, 0.5 }, Vector3::Forward, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },       // 7

            // -z
            { { 0.5, -0.5f, -0.5 }, Vector3::Backward, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },       // 5
            { { 0.5, 0.5f, -0.5 }, Vector3::Backward, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },        // 1
            { { -0.5, 0.5f, -0.5 }, Vector3::Backward, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },       // 2
            { { -0.5, -0.5f, -0.5 }, Vector3::Backward, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },      // 6

            // -y
            { { 0.5, -0.5f, 0.5 }, Vector3::Up, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },        // 4
            { { 0.5, -0.5f, -0.5 }, Vector3::Up, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },       // 5
            { { -0.5, -0.5f, -0.5 }, Vector3::Up, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },      // 6
            { { -0.5, -0.5f, 0.5 }, Vector3::Up, { 0, 0 }, { 1.0f, 1.0f, 1.0f } }        // 7
        };

        const std::vector<uint32_t> indices
        {
            0,  1,  2,  2,  3,  0,  // +y
            4,  5,  6,  6,  7,  4,  // +x
            8,  9,  10, 10, 11, 8,  // -x
            12, 13, 14, 13, 15, 14, // +z
            16, 17, 18, 18, 19, 16, // -z
            20, 21, 22, 22, 23, 20  // -y
        };

        return source(vertices, std::vector<std::vector<uint32_t>>(), indices, std::vector<TransparentTriangle>(), std::vector<Triangle>());
    }

    std::unique_ptr<IMesh> create_sprite_mesh(const IMesh::Source& source, const trlevel::tr_sprite_texture& sprite, Matrix& scale, Vector3& offset, SpriteOffsetMode offset_mode)
    {
        // Calculate UVs.
        float u = static_cast<float>(sprite.x) / 256.0f;
        float v = static_cast<float>(sprite.y) / 256.0f;
        float width = static_cast<float>((sprite.Width - 255) / 256) / 256.0f;
        float height = static_cast<float>((sprite.Height - 255) / 256) / 256.0f;

        // Generate quad.
        using namespace DirectX::SimpleMath;
        std::vector<MeshVertex> vertices
        {
            { Vector3(-0.5f, -0.5f, 0), Vector3::Zero, Vector2(u, v + height), Color(1,1,1,1)  },
            { Vector3(0.5f, -0.5f, 0), Vector3::Zero, Vector2(u + width, v + height), Color(1,1,1,1) },
            { Vector3(-0.5f, 0.5f, 0), Vector3::Zero, Vector2(u, v), Color(1,1,1,1) },
            { Vector3(0.5f, 0.5f, 0), Vector3::Zero, Vector2(u + width, v), Color(1,1,1,1) },
        };

        std::vector<TransparentTriangle> transparent_triangles
        {
            { vertices[0].pos, vertices[1].pos, vertices[2].pos, vertices[0].uv, vertices[1].uv, vertices[2].uv, sprite.Tile, TransparentTriangle::Mode::Normal },
            { vertices[2].pos, vertices[1].pos, vertices[3].pos, vertices[2].uv, vertices[1].uv, vertices[3].uv, sprite.Tile, TransparentTriangle::Mode::Normal },
        };

        std::vector<Triangle> collision_triangles;

        float object_width = static_cast<float>(sprite.RightSide - sprite.LeftSide) / trlevel::Scale_X;
        float object_height = static_cast<float>(sprite.BottomSide - sprite.TopSide) / trlevel::Scale_Y;
        scale = Matrix::CreateScale(object_width, object_height, 1);

        if (offset_mode == SpriteOffsetMode::RoomSprite)
        {
            offset = Vector3(0, (1 - object_height) * 0.5f, 0);
        }
        else
        {
            offset = Vector3(0, object_height / -2.0f, 0);
        }

        return source(std::vector<MeshVertex>(), std::vector<std::vector<uint32_t>>(), std::vector<uint32_t>(), transparent_triangles, collision_triangles);
    }

    std::unique_ptr<IMesh> create_sprite_mesh(const IMesh::Source& source, const trlevel::tr_sprite_texture& sprite, Matrix& scale, Matrix& offset, SpriteOffsetMode offset_mode)
    {
        Vector3 offset_vector;
        auto mesh = create_sprite_mesh(source, sprite, scale, offset_vector, offset_mode);
        offset = Matrix::CreateTranslation(offset_vector);
        return std::move(mesh);
    }

    void process_textured_rectangles(
        trlevel::LevelVersion level_version,
        const std::vector<trlevel::tr4_mesh_face4>& rectangles,
        const std::vector<trlevel::tr_vertex>& input_vertices,
        const ILevelTextureStorage& texture_storage,
        std::vector<MeshVertex>& output_vertices,
        std::vector<std::vector<uint32_t>>& output_indices,
        std::vector<TransparentTriangle>& transparent_triangles,
        std::vector<Triangle>& collision_triangles,
        bool transparent_collision)
    {
        using namespace trlevel;

        for (const auto& rect : rectangles)
        {
            std::array<Vector3, 4> verts;
            for (int i = 0; i < 4; ++i)
            {
                verts[i] = convert_vertex(input_vertices[rect.vertices[i]]);
            }

            const uint16_t texture = rect.texture & texture_mask(level_version);

            std::array<Vector2, 4> uvs;
            for (auto i = 0u; i < uvs.size(); ++i)
            {
                uvs[i] = texture_storage.uv(texture, i);
            }

            const bool double_sided = rect.texture & 0x8000;

            TransparentTriangle::Mode transparency_mode;
            if (determine_transparency(texture_storage.attribute(texture), rect.effects, transparency_mode))
            {
                transparent_triangles.emplace_back(verts[0], verts[1], verts[2], uvs[0], uvs[1], uvs[2], texture_storage.tile(texture), transparency_mode);
                transparent_triangles.emplace_back(verts[2], verts[3], verts[0], uvs[2], uvs[3], uvs[0], texture_storage.tile(texture), transparency_mode);
                if (transparent_collision)
                {
                    collision_triangles.emplace_back(verts[0], verts[1], verts[2]);
                    collision_triangles.emplace_back(verts[2], verts[3], verts[0]);
                }

                if (double_sided)
                {

                    transparent_triangles.emplace_back(verts[2], verts[1], verts[0], uvs[2], uvs[1], uvs[0], texture_storage.tile(texture), transparency_mode);
                    transparent_triangles.emplace_back(verts[0], verts[3], verts[2], uvs[0], uvs[3], uvs[2], texture_storage.tile(texture), transparency_mode);
                    if (transparent_collision)
                    {
                        collision_triangles.emplace_back(verts[2], verts[1], verts[0]);
                        collision_triangles.emplace_back(verts[0], verts[3], verts[2]);
                    }
                }
                continue;
            }

            const uint32_t base = static_cast<uint32_t>(output_vertices.size());
            const auto normal = calculate_normal(&verts[0]);
            for (int i = 0; i < 4; ++i)
            {
                output_vertices.push_back({ verts[i], normal, uvs[i], Color(1,1,1,1) });
            }

            auto& tex_indices = output_indices[texture_storage.tile(texture)];
            tex_indices.push_back(base);
            tex_indices.push_back(base + 1);
            tex_indices.push_back(base + 2);
            tex_indices.push_back(base + 2);
            tex_indices.push_back(base + 3);
            tex_indices.push_back(base + 0);

            if (double_sided)
            {
                tex_indices.push_back(base + 2);
                tex_indices.push_back(base + 1);
                tex_indices.push_back(base);
                tex_indices.push_back(base);
                tex_indices.push_back(base + 3);
                tex_indices.push_back(base + 2);
            }

            collision_triangles.emplace_back(verts[0], verts[1], verts[2]);
            collision_triangles.emplace_back(verts[2], verts[3], verts[0]);
            if (double_sided)
            {
                collision_triangles.emplace_back(verts[2], verts[1], verts[0]);
                collision_triangles.emplace_back(verts[0], verts[3], verts[2]);
            }
        }
    }

    void process_textured_triangles(
        trlevel::LevelVersion level_version,
        const std::vector<trlevel::tr4_mesh_face3>& triangles,
        const std::vector<trlevel::tr_vertex>& input_vertices,
        const ILevelTextureStorage& texture_storage,
        std::vector<MeshVertex>& output_vertices,
        std::vector<std::vector<uint32_t>>& output_indices,
        std::vector<TransparentTriangle>& transparent_triangles,
        std::vector<Triangle>& collision_triangles,
        bool transparent_collision)
    {
        for (const auto& tri : triangles)
        {
            std::array<Vector3, 3> verts;
            for (int i = 0; i < 3; ++i)
            {
                verts[i] = convert_vertex(input_vertices[tri.vertices[i]]);
            }

            // Select UVs - otherwise they will be 0.
            const uint16_t texture = tri.texture & texture_mask(level_version);
            std::array<Vector2, 3> uvs;
            for (auto i = 0u; i < uvs.size(); ++i)
            {
                uvs[i] = texture_storage.uv(texture, i);
            }

            const bool double_sided = tri.texture & 0x8000;

            TransparentTriangle::Mode transparency_mode;
            if (determine_transparency(texture_storage.attribute(texture), tri.effects, transparency_mode))
            {
                transparent_triangles.emplace_back(verts[0], verts[1], verts[2], uvs[0], uvs[1], uvs[2], texture_storage.tile(texture), transparency_mode);
                if (transparent_collision)
                {
                    collision_triangles.emplace_back(verts[0], verts[1], verts[2]);
                }
                if (double_sided)
                {
                    transparent_triangles.emplace_back(verts[2], verts[1], verts[0], uvs[2], uvs[1], uvs[0], texture_storage.tile(texture), transparency_mode);
                    if (transparent_collision)
                    {
                        collision_triangles.emplace_back(verts[2], verts[1], verts[0]);
                    }
                }
                continue;
            }

            const uint32_t base = static_cast<uint32_t>(output_vertices.size());
            const auto normal = calculate_normal(&verts[0]);
            for (int i = 0; i < 3; ++i)
            {
                output_vertices.push_back({ verts[i], normal, uvs[i], Color(1,1,1,1) });
            }

            auto& tex_indices = output_indices[texture_storage.tile(texture)];
            tex_indices.push_back(base);
            tex_indices.push_back(base + 1);
            tex_indices.push_back(base + 2);
            if (double_sided)
            {
                tex_indices.push_back(base + 2);
                tex_indices.push_back(base + 1);
                tex_indices.push_back(base);
            }

            collision_triangles.emplace_back(verts[0], verts[1], verts[2]);
            if (double_sided)
            {
                collision_triangles.emplace_back(verts[2], verts[1], verts[0]);
            }
        }
    }

    void process_coloured_rectangles(
        const std::vector<trlevel::tr_face4>& rectangles,
        const std::vector<trlevel::tr_vertex>& input_vertices,
        const ILevelTextureStorage& texture_storage,
        std::vector<MeshVertex>& output_vertices,
        std::vector<uint32_t>& output_indices,
        std::vector<Triangle>& collision_triangles)
    {
        for (const auto& rect : rectangles)
        {
            const uint16_t texture = rect.texture & 0x7fff;
            const bool double_sided = rect.texture & 0x8000;

            std::array<Vector3, 4> verts;
            for (int i = 0; i < 4; ++i)
            {
                verts[i] = convert_vertex(input_vertices[rect.vertices[i]]);
            }

            const uint32_t base = static_cast<uint32_t>(output_vertices.size());
            const auto normal = calculate_normal(&verts[0]);
            for (int i = 0; i < 4; ++i)
            {
                output_vertices.push_back({ verts[i], normal, Vector2::Zero, texture_storage.palette_from_texture(texture) });
            }

            output_indices.push_back(base);
            output_indices.push_back(base + 1);
            output_indices.push_back(base + 2);
            output_indices.push_back(base + 2);
            output_indices.push_back(base + 3);
            output_indices.push_back(base + 0);
            if (double_sided)
            {
                output_indices.push_back(base + 2);
                output_indices.push_back(base + 1);
                output_indices.push_back(base);
                output_indices.push_back(base);
                output_indices.push_back(base + 3);
                output_indices.push_back(base + 2);
            }

            collision_triangles.emplace_back(verts[0], verts[1], verts[2]);
            collision_triangles.emplace_back(verts[2], verts[3], verts[0]);
            if (double_sided)
            {
                collision_triangles.emplace_back(verts[2], verts[1], verts[0]);
                collision_triangles.emplace_back(verts[0], verts[3], verts[2]);
            }
        }
    }

    void process_coloured_triangles(
        const std::vector<trlevel::tr_face3>& triangles,
        const std::vector<trlevel::tr_vertex>& input_vertices,
        const ILevelTextureStorage& texture_storage,
        std::vector<MeshVertex>& output_vertices,
        std::vector<uint32_t>& output_indices,
        std::vector<Triangle>& collision_triangles)
    {
        for (const auto& tri : triangles)
        {
            const uint16_t texture = tri.texture & 0x7fff;
            const bool double_sided = tri.texture & 0x8000;

            std::array<Vector3, 3> verts;
            for (int i = 0; i < 3; ++i)
            {
                verts[i] = convert_vertex(input_vertices[tri.vertices[i]]);
            }

            const uint32_t base = static_cast<uint32_t>(output_vertices.size());
            const auto normal = calculate_normal(&verts[0]);
            for (int i = 0; i < 3; ++i)
            {
                output_vertices.push_back({ verts[i], normal, Vector2::Zero, texture_storage.palette_from_texture(texture) });
            }

            output_indices.push_back(base);
            output_indices.push_back(base + 1);
            output_indices.push_back(base + 2);
            collision_triangles.emplace_back(verts[0], verts[1], verts[2]);

            if (double_sided)
            {
                output_indices.push_back(base + 2);
                output_indices.push_back(base + 1);
                output_indices.push_back(base);
                collision_triangles.emplace_back(verts[2], verts[1], verts[0]);
            }
        }
    }

    Vector3 convert_vertex(const trlevel::tr_vertex& vertex)
    {
        return Vector3(vertex.x / trlevel::Scale_X, vertex.y / trlevel::Scale_Y, vertex.z / trlevel::Scale_Z);
    }
}
