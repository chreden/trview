#include "IMesh.h"
#include <random>
#include <trlevel/trtypes.h>
#include <ranges>

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        const uint16_t Texture_Mask = 0x3fff;

        Vector3 convert_vertex(const Vector3& vertex)
        {
            return vertex / trlevel::Scale;
        }

        Vector3 calculate_normal(const Vector3* const vertices)
        {
            auto first = vertices[1] - vertices[0];
            auto second = vertices[2] - vertices[0];
            first.Normalize();
            second.Normalize();
            return first.Cross(second);
        }

        std::shared_ptr<IMesh> create_sprite_mesh(const IMesh::Source& source,
            uint8_t x,
            uint8_t y,
            uint16_t w,
            uint16_t h,
            uint32_t tile,
            int16_t left,
            int16_t right,
            int16_t top,
            int16_t bottom,
            Matrix& scale,
            Vector3& offset,
            SpriteOffsetMode offset_mode)
        {
            // Calculate UVs.
            float u = static_cast<float>(x) / 256.0f;
            float v = static_cast<float>(y) / 256.0f;
            float width = static_cast<float>((w - 255) / 256) / 256.0f;
            float height = static_cast<float>((h - 255) / 256) / 256.0f;

            // Generate quad.
            using namespace DirectX::SimpleMath;
            std::vector<MeshVertex> vertices
            {
                { Vector3(-0.5f, -0.5f, 0), Vector3::Zero, Vector2(u + width, v + height), Color(1,1,1,1)  },
                { Vector3(0.5f, -0.5f, 0), Vector3::Zero, Vector2(u, v + height), Color(1,1,1,1) },
                { Vector3(-0.5f, 0.5f, 0), Vector3::Zero, Vector2(u + width, v), Color(1,1,1,1) },
                { Vector3(0.5f, 0.5f, 0), Vector3::Zero, Vector2(u, v), Color(1,1,1,1) },
            };

            std::vector<TransparentTriangle> transparent_triangles
            {
                { vertices[0].pos, vertices[1].pos, vertices[2].pos, vertices[0].uv, vertices[1].uv, vertices[2].uv, tile, TransparentTriangle::Mode::Normal, Colour::White, Colour::White, Colour::White },
                { vertices[2].pos, vertices[1].pos, vertices[3].pos, vertices[2].uv, vertices[1].uv, vertices[3].uv, tile, TransparentTriangle::Mode::Normal, Colour::White, Colour::White, Colour::White },
            };

            std::vector<Triangle> collision_triangles
            {
                Triangle(vertices[0].pos, vertices[1].pos, vertices[2].pos),
                Triangle(vertices[2].pos, vertices[1].pos, vertices[3].pos)
            };

            float object_width = static_cast<float>(right - left) / trlevel::Scale_X;
            float object_height = static_cast<float>(bottom - top) / trlevel::Scale_Y;
            scale = Matrix::CreateScale(object_width, object_height, 1);

            if (offset_mode == SpriteOffsetMode::RoomSprite)
            {
                if (bottom != top)
                {
                    const float y_factor = (-0.5f + ((fabs(static_cast<float>(top)) / (bottom - top))));
                    offset = Vector3(0, -object_height * y_factor, 0);
                }
            }
            else
            {
                offset = Vector3(0, object_height / -2.0f, 0);
            }

            return source(std::vector<MeshVertex>(), std::vector<std::vector<uint32_t>>(), std::vector<uint32_t>(), transparent_triangles, collision_triangles);
        }

        void adjust_rect_uvs_tr1_1996_pc(std::array<Vector2, 4>& uvs, uint16_t texture)
        {
            int16_t signed_texture = static_cast<int16_t>(texture);
            if (signed_texture < 0)
            {
                std::swap(uvs[0], uvs[1]);
                std::swap(uvs[2], uvs[3]);
            }
        }

        void adjust_tri_uvs_tr1_1996_pc(std::array<Vector2, 4>& uvs, uint16_t texture)
        {
            const uint16_t texture_operation = (texture & 0x6000) >> 13;
            switch (texture_operation)
            {
                case 1:
                {
                    std::swap(uvs[1], uvs[2]);
                    break;
                }
                case 2:
                {
                    std::swap(uvs[0], uvs[3]);
                    std::swap(uvs[1], uvs[2]);
                    break;
                }
                case 3:
                {
                    std::swap(uvs[0], uvs[3]);
                    break;
                }
            }
        }

        constexpr uint16_t get_raw_texture_operation(uint16_t texture)
        {
            return (texture & 0xE000) >> 13;
        }

        void adjust_tri_uvs_tr1_saturn(std::array<Vector2, 4>& uvs, uint16_t texture)
        {
            const uint16_t texture_operation = get_raw_texture_operation(texture);
            switch (texture_operation)
            {
                case 0:
                case 2:
                {
                    break;
                }
                case 1:
                case 3:
                {
                    const Vector2 v0 = uvs[2];
                    const Vector2 v1 = uvs[0];
                    const Vector2 v2 = uvs[3];
                    uvs[0] = v0;
                    uvs[1] = v1;
                    uvs[2] = v2;
                    break;
                }
                case 4:
                case 6:
                {
                    const Vector2 v0 = uvs[2];
                    const Vector2 v1 = uvs[3];
                    const Vector2 v2 = uvs[0];
                    uvs[0] = v0;
                    uvs[1] = v1;
                    uvs[2] = v2;
                    break;
                }
                case 5:
                case 7:
                {
                    std::swap(uvs[1], uvs[2]);
                    break;
                }
            }
        }

        /// <summary>
        /// Convert Saturn texture into a colour value.
        /// </summary>
        DirectX::SimpleMath::Color colour_from_texture(uint16_t t)
        {
            const uint16_t b = (t & 0x7c00) >> 10;
            const uint16_t g = (t & 0x03e0) >> 5;
            const uint16_t r = t & 0x001f;
            return Color(r / 31.0f, g / 31.0f, b / 31.0f);
        }

        bool needs_rect_texture_abs(trlevel::PlatformAndVersion version)
        {
            return is_tr1_pc_may_1996(version) || is_tr1_version_21(version);
        }

        bool needs_tri_texture_mask(trlevel::PlatformAndVersion version)
        {
            return is_tr1_pc_may_1996(version) || is_tr1_version_21(version) || version.platform == trlevel::Platform::Saturn;
        }

        bool needs_rect_uv_adjustment(trlevel::PlatformAndVersion version)
        {
            return is_tr1_pc_may_1996(version) || is_tr1_version_21(version) || version.platform == trlevel::Platform::Saturn;
        }

        bool needs_tri_uv_adjustment(trlevel::PlatformAndVersion version)
        {
            return is_tr1_pc_may_1996(version) || is_tr1_version_21(version);
        }

        bool needs_rect_uv_swap(trlevel::PlatformAndVersion version)
        {
            return version.platform == trlevel::Platform::PSX ||
                is_tr1_pc_may_1996(version) ||
                is_tr1_version_21(version) ||
                version.platform == trlevel::Platform::Saturn;
        }
    }

    IMesh::~IMesh()
    {
    }

    std::shared_ptr<IMesh> create_mesh(const trlevel::tr_mesh& mesh, const IMesh::Source& source, const ILevelTextureStorage& texture_storage, const trlevel::PlatformAndVersion& platform_and_version, bool transparent_collision)
    {
        std::vector<std::vector<uint32_t>> indices(texture_storage.num_tiles());
        std::vector<MeshVertex> vertices;
        std::vector<uint32_t> untextured_indices;
        std::vector<TransparentTriangle> transparent_triangles;
        std::vector<Triangle> collision_triangles;

        std::vector<trlevel::trview_vertex> in_vertices = mesh.vertices | std::views::transform([](auto&& v) -> trlevel::trview_vertex
            {
                return { .vertex = { static_cast<float>(v.x), static_cast<float>(v.y), static_cast<float>(v.z) }, .colour = Color(1, 1, 1, 1) };
            }) | std::ranges::to<std::vector>();

        auto rectangles = convert_rectangles_2(mesh.textured_rectangles);
        preprocess_textured_rectangles(rectangles, texture_storage);
        process_textured_rectangles(rectangles, in_vertices, vertices, indices, transparent_triangles, collision_triangles, transparent_collision);
        auto triangles = convert_triangles_2(mesh.textured_triangles);
        preprocess_textured_triangles(triangles, texture_storage);
        process_textured_triangles(triangles, in_vertices, vertices, indices, transparent_triangles, collision_triangles, transparent_collision);
        process_coloured_rectangles(mesh.coloured_rectangles, in_vertices, texture_storage, vertices, untextured_indices, collision_triangles, platform_and_version);
        process_coloured_triangles(mesh.coloured_triangles, in_vertices, texture_storage, vertices, untextured_indices, collision_triangles, platform_and_version);

        return source(vertices, indices, untextured_indices, transparent_triangles, collision_triangles);
    }

    std::shared_ptr<IMesh> create_cube_mesh(const IMesh::Source& source)
    {
        const std::vector<MeshVertex> vertices
        {
            // Body:
            // + y
            { { -0.5, 0.5f, -0.5 }, Vector3::Down, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },       // 2
            { { 0.5, 0.5f, -0.5 }, Vector3::Down, { 1, 0 }, { 1.0f, 1.0f, 1.0f } },        // 1
            { { 0.5, 0.5f, 0.5 }, Vector3::Down, { 1, 1 }, { 1.0f, 1.0f, 1.0f } },         // 0
            { { -0.5, 0.5f, 0.5 }, Vector3::Down, { 0, 1 }, { 1.0f, 1.0f, 1.0f } },        // 3

            // +x
            { { 0.5, -0.5f, -0.5 }, Vector3::Left, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },       // 5
            { { 0.5, -0.5f, 0.5 }, Vector3::Left, { 1, 0 }, { 1.0f, 1.0f, 1.0f } },        // 4
            { { 0.5, 0.5f, 0.5 }, Vector3::Left, { 1, 1 }, { 1.0f, 1.0f, 1.0f } },         // 0
            { { 0.5, 0.5f, -0.5 }, Vector3::Left, { 0, 1 }, { 1.0f, 1.0f, 1.0f } },        // 1

            // -x 
            { { -0.5, 0.5f, -0.5 }, Vector3::Right, { 1, 1 }, { 1.0f, 1.0f, 1.0f } },       // 2
            { { -0.5, 0.5f, 0.5 }, Vector3::Right, { 0, 1 }, { 1.0f, 1.0f, 1.0f } },        // 3
            { { -0.5, -0.5f, 0.5 }, Vector3::Right, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },       // 7
            { { -0.5, -0.5f, -0.5 }, Vector3::Right, { 1, 0 }, { 1.0f, 1.0f, 1.0f } },      // 6

            // +z
            { { 0.5, 0.5f, 0.5 }, Vector3::Forward, { 0, 1 }, { 1.0f, 1.0f, 1.0f } },         // 0
            { { 0.5, -0.5f, 0.5 }, Vector3::Forward, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },        // 4
            { { -0.5, 0.5f, 0.5 }, Vector3::Forward, { 1, 1 }, { 1.0f, 1.0f, 1.0f } },        // 3
            { { -0.5, -0.5f, 0.5 }, Vector3::Forward, { 1, 0 }, { 1.0f, 1.0f, 1.0f } },       // 7

            // -z
            { { 0.5, -0.5f, -0.5 }, Vector3::Backward, { 1, 0 }, { 1.0f, 1.0f, 1.0f } },       // 5
            { { 0.5, 0.5f, -0.5 }, Vector3::Backward, { 1, 1 }, { 1.0f, 1.0f, 1.0f } },        // 1
            { { -0.5, 0.5f, -0.5 }, Vector3::Backward, { 0, 1 }, { 1.0f, 1.0f, 1.0f } },       // 2
            { { -0.5, -0.5f, -0.5 }, Vector3::Backward, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },      // 6

            // -y
            { { 0.5, -0.5f, 0.5 }, Vector3::Up, { 1, 0 }, { 1.0f, 1.0f, 1.0f } },        // 4
            { { 0.5, -0.5f, -0.5 }, Vector3::Up, { 1, 1 }, { 1.0f, 1.0f, 1.0f } },       // 5
            { { -0.5, -0.5f, -0.5 }, Vector3::Up, { 0, 1 }, { 1.0f, 1.0f, 1.0f } },      // 6
            { { -0.5, -0.5f, 0.5 }, Vector3::Up, { 0, 0 }, { 1.0f, 1.0f, 1.0f } },        // 7
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

        return source(vertices, std::vector<std::vector<uint32_t>>(), indices, std::vector<TransparentTriangle>(), {});
    }

    std::shared_ptr<IMesh> create_sphere_mesh(const IMesh::Source& source, uint32_t stacks, uint32_t slices)
    {
        constexpr float Pi2 = 6.283185307f;
        // Rotation around X per stack
        const float per_stack = Pi2 / stacks;
        // Rotation areound Y per slice
        const float per_slice = Pi2 / slices;
        const Matrix slice_rotation = Matrix::CreateRotationY(per_slice);

        std::vector<MeshVertex> points;

        const Vector3 top(0, -0.5f, 0);
        points.push_back(MeshVertex{ top, Vector3::Down, Vector2::Zero, Colour::White });

        for (uint32_t stack = 0; stack < stacks; ++stack)
        {
            Vector3 point = Vector3::Transform(top, Matrix::CreateRotationX(per_stack * (stack + 1)));
            for (uint32_t slice = 0; slice < slices; ++slice)
            {
                Vector3 normal = point;
                normal.Normalize();
                points.push_back({ point, normal, Vector2::Zero, Colour::White });
                point = Vector3::Transform(point, slice_rotation);
            }
        }

        std::vector<uint32_t> indices;
        for (uint32_t slice = 0; slice < slices; ++slice)
        {
            indices.push_back(0);
            indices.push_back(slice + 1);
            indices.push_back(slice == slices - 1 ? 1 : slice + 2);
        }

        for (uint32_t stack = 0; stack < stacks; ++stack)
        {
            uint32_t b = 1 + stack * slices;
            for (uint32_t slice = 0; slice < slices; ++slice)
            {
                const uint32_t top_right = (slice == slices - 1 ? 0 : slice + 1);
                const uint32_t bottom_right = (slice == slices - 1 ? 0 : slice + 1);

                indices.push_back(b + slice);
                indices.push_back(b + slice + slices);
                indices.push_back(b + slices + bottom_right);
                indices.push_back(b + slice);
                indices.push_back(b + slices + bottom_right);
                indices.push_back(b + top_right);
            }
        }

        return source(points, std::vector<std::vector<uint32_t>>(), indices, std::vector<TransparentTriangle>(), std::vector<Triangle>());
    }

    std::shared_ptr<IMesh> create_sprite_mesh(const IMesh::Source& source, const std::optional<trlevel::tr_sprite_texture>& sprite, Matrix& scale, Vector3& offset, SpriteOffsetMode offset_mode)
    {
        if (sprite)
        {
            const auto& s = sprite.value();
            return create_sprite_mesh(source, s.x + 1, s.y + 1, s.Width - 1, s.Height - 1, s.Tile, s.LeftSide, s.RightSide, s.TopSide, s.BottomSide, scale, offset, offset_mode);
        }
        return create_sprite_mesh(source, 0, 0, 0, 0, TransparentTriangle::Untextured, -256, 256, -256, 256, scale, offset, offset_mode);
    }

    std::shared_ptr<IMesh> create_sprite_mesh(const IMesh::Source& source, const std::optional<trlevel::tr_sprite_texture>& sprite, Matrix& scale, Matrix& offset, SpriteOffsetMode offset_mode)
    {
        Vector3 offset_vector;
        auto mesh = create_sprite_mesh(source, sprite, scale, offset_vector, offset_mode);
        offset = Matrix::CreateTranslation(offset_vector);
        return mesh;
    }

    void preprocess_textured_rectangles(
        std::vector<trlevel::trview_mesh_face4>& rectangles,
        const ILevelTextureStorage& texture_storage)
    {
        using namespace trlevel;
        const auto version = texture_storage.platform_and_version();

        uint16_t previous_texture = 0;
        for (auto& rect : rectangles)
        {
            uint16_t texture = rect.texture & Texture_Mask;
            if (needs_rect_texture_abs(version))
            {
                texture = static_cast<uint16_t>(std::abs(static_cast<int16_t>(rect.texture)));
            }

            if (texture >= texture_storage.num_object_textures())
            {
                texture = previous_texture;
            }
            previous_texture = texture;
            rect.texture = texture_storage.tile(texture);

            for (auto i = 0u; i < 4; ++i)
            {
                rect.uvs[i] = texture_storage.uv(texture, i);
            }

            if (needs_rect_uv_adjustment(version) && static_cast<int16_t>(rect.texture) < 0)
            {
                adjust_rect_uvs_tr1_1996_pc(rect.uvs, static_cast<uint16_t>(rect.texture));
            }

            if (needs_rect_uv_swap(version))
            {
                std::swap(rect.uvs[2], rect.uvs[3]);
            }

            rect.double_sided = texture_storage.platform_and_version().platform != Platform::Saturn && (rect.texture & 0x8000);
            TransparentTriangle::Mode transparency_mode;
            if (determine_transparency(
                texture_storage.platform_and_version().platform == Platform::Saturn ? static_cast<uint16_t>(rect.effects) : texture_storage.attribute(texture),
                texture_storage.platform_and_version().platform == Platform::Saturn ? 0 : static_cast<uint16_t>(rect.effects), transparency_mode))
            {
                rect.blend_mode = transparency_mode == TransparentTriangle::Mode::Normal ? BlendMode::Normal : BlendMode::Additive;
            }
        }
    }

    void process_textured_rectangles(
        const std::vector<trlevel::trview_mesh_face4>& rectangles,
        const std::vector<trlevel::trview_vertex>& input_vertices,
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
            std::array<Color, 4> colors;
            std::array<Vector2, 4> uvs;
            for (int i = 0; i < 4; ++i)
            {
                verts[i] = convert_vertex(input_vertices[rect.vertices[i]].vertex);
                colors[i] = input_vertices[rect.vertices[i]].colour;
                uvs[i] = rect.uvs[i];
            }

            if (rect.blend_mode != BlendMode::None)
            {
                const auto transparency_mode = rect.blend_mode == BlendMode::Normal ? TransparentTriangle::Mode::Normal : TransparentTriangle::Mode::Additive;
                transparent_triangles.emplace_back(verts[0], verts[1], verts[2], uvs[0], uvs[1], uvs[2], rect.texture, transparency_mode, colors[0], colors[1], colors[2]);
                transparent_triangles.emplace_back(verts[2], verts[3], verts[0], uvs[2], uvs[3], uvs[0], rect.texture, transparency_mode, colors[2], colors[3], colors[0]);
                if (transparent_collision)
                {
                    collision_triangles.emplace_back(verts[0], verts[1], verts[2]);
                    collision_triangles.emplace_back(verts[2], verts[3], verts[0]);
                }

                if (rect.double_sided)
                {

                    transparent_triangles.emplace_back(verts[2], verts[1], verts[0], uvs[2], uvs[1], uvs[0], rect.texture, transparency_mode, colors[2], colors[1], colors[0]);
                    transparent_triangles.emplace_back(verts[0], verts[3], verts[2], uvs[0], uvs[3], uvs[2], rect.texture, transparency_mode, colors[0], colors[3], colors[2]);
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
                output_vertices.push_back({ verts[i], normal, uvs[i], colors[i] });
            }

            auto& tex_indices = output_indices[rect.texture];
            tex_indices.push_back(base);
            tex_indices.push_back(base + 1);
            tex_indices.push_back(base + 2);
            tex_indices.push_back(base + 2);
            tex_indices.push_back(base + 3);
            tex_indices.push_back(base + 0);

            if (rect.double_sided)
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
            if (rect.double_sided)
            {
                collision_triangles.emplace_back(verts[2], verts[1], verts[0]);
                collision_triangles.emplace_back(verts[0], verts[3], verts[2]);
            }
        }
    }

    void preprocess_textured_triangles(
        std::vector<trlevel::trview_mesh_face3>& triangles,
        const ILevelTextureStorage& texture_storage)
    {
        using namespace trlevel;
        const auto version = texture_storage.platform_and_version();

        uint16_t previous_texture = 0;
        for (auto& tri : triangles)
        {
            uint16_t texture = tri.texture & Texture_Mask;

            if (needs_tri_texture_mask(version))
            {
                texture = tri.texture & 0x1FFF;
            }

            if (texture >= texture_storage.num_object_textures())
            {
                texture = previous_texture;
            }
            previous_texture = texture;
            tri.texture = texture_storage.tile(texture);

            std::array<Vector2, 4> uvs;
            for (auto i = 0u; i < uvs.size(); ++i)
            {
                uvs[i] = texture_storage.uv(texture, i);
            }

            if (needs_tri_uv_adjustment(version))
            {
                adjust_tri_uvs_tr1_1996_pc(uvs, static_cast<uint16_t>(tri.texture));
            }

            if (version.platform == trlevel::Platform::Saturn)
            {
                adjust_tri_uvs_tr1_saturn(uvs, static_cast<uint16_t>(tri.texture));
            }

            std::copy(uvs.begin(), uvs.begin() + 3, tri.uvs);

            tri.double_sided = texture_storage.platform_and_version().platform != Platform::Saturn && (tri.texture & 0x8000);
            TransparentTriangle::Mode transparency_mode;
            if (determine_transparency(
                texture_storage.platform_and_version().platform == Platform::Saturn ? static_cast<uint16_t>(tri.effects) : texture_storage.attribute(texture),
                texture_storage.platform_and_version().platform == Platform::Saturn ? 0 : static_cast<uint16_t>(tri.effects), transparency_mode))
            {
                tri.blend_mode = transparency_mode == TransparentTriangle::Mode::Normal ? BlendMode::Normal : BlendMode::Additive;
            }
        }
    }

    void process_textured_triangles(
        const std::vector<trlevel::trview_mesh_face3>& triangles,
        const std::vector<trlevel::trview_vertex>& input_vertices,
        std::vector<MeshVertex>& output_vertices,
        std::vector<std::vector<uint32_t>>& output_indices,
        std::vector<TransparentTriangle>& transparent_triangles,
        std::vector<Triangle>& collision_triangles,
        bool transparent_collision)
    {
        using namespace trlevel;

        for (const auto& tri : triangles)
        {
            std::array<Vector3, 3> verts;
            std::array<Color, 3> colors;
            std::array<Vector2, 3> uvs;
            for (int i = 0; i < 3; ++i)
            {
                verts[i] = convert_vertex(input_vertices[tri.vertices[i]].vertex);
                colors[i] = input_vertices[tri.vertices[i]].colour;
                uvs[i] = tri.uvs[i];
            }

            if (tri.blend_mode != BlendMode::None)
            {
                const auto transparency_mode = tri.blend_mode == BlendMode::Normal ? TransparentTriangle::Mode::Normal : TransparentTriangle::Mode::Additive;
                transparent_triangles.emplace_back(verts[0], verts[1], verts[2], uvs[0], uvs[1], uvs[2], tri.texture, transparency_mode, colors[0], colors[1], colors[2]);
                if (transparent_collision)
                {
                    collision_triangles.emplace_back(verts[0], verts[1], verts[2]);
                }
                if (tri.double_sided)
                {
                    transparent_triangles.emplace_back(verts[2], verts[1], verts[0], uvs[2], uvs[1], uvs[0], tri.texture, transparency_mode, colors[2], colors[1], colors[0]);
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
                output_vertices.push_back({ verts[i], normal, uvs[i], colors[i] });
            }

            auto& tex_indices = output_indices[tri.texture];
            tex_indices.push_back(base);
            tex_indices.push_back(base + 1);
            tex_indices.push_back(base + 2);
            if (tri.double_sided)
            {
                tex_indices.push_back(base + 2);
                tex_indices.push_back(base + 1);
                tex_indices.push_back(base);
            }

            collision_triangles.emplace_back(verts[0], verts[1], verts[2]);
            if (tri.double_sided)
            {
                collision_triangles.emplace_back(verts[2], verts[1], verts[0]);
            }
        }
    }

    void process_coloured_rectangles(
        const std::vector<trlevel::tr_face4>& rectangles,
        const std::vector<trlevel::trview_vertex>& input_vertices,
        const ILevelTextureStorage& texture_storage,
        std::vector<MeshVertex>& output_vertices,
        std::vector<uint32_t>& output_indices,
        std::vector<Triangle>& collision_triangles,
        const trlevel::PlatformAndVersion& platform_and_version)
    {
        for (const auto& rect : rectangles)
        {
            const uint16_t texture = rect.texture & 0x7fff;
            const bool double_sided = platform_and_version.platform == trlevel::Platform::Saturn ? false : rect.texture & 0x8000;

            std::array<Vector3, 4> verts;
            for (int i = 0; i < 4; ++i)
            {
                verts[i] = convert_vertex(input_vertices[rect.vertices[i]].vertex);
            }

            const uint32_t base = static_cast<uint32_t>(output_vertices.size());
            const auto normal = calculate_normal(&verts[0]);
            for (int i = 0; i < 4; ++i)
            {
                output_vertices.push_back({ verts[i], normal, Vector2::Zero, platform_and_version.platform == trlevel::Platform::Saturn ? colour_from_texture(rect.texture) : texture_storage.palette_from_texture(texture) });
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
        const std::vector<trlevel::trview_vertex>& input_vertices,
        const ILevelTextureStorage& texture_storage,
        std::vector<MeshVertex>& output_vertices,
        std::vector<uint32_t>& output_indices,
        std::vector<Triangle>& collision_triangles,
        const trlevel::PlatformAndVersion& platform_and_version)
    {
        for (const auto& tri : triangles)
        {
            const uint16_t texture = tri.texture & 0x7fff;
            const bool double_sided = platform_and_version.platform == trlevel::Platform::Saturn ? false : tri.texture & 0x8000;

            std::array<Vector3, 3> verts;
            for (int i = 0; i < 3; ++i)
            {
                verts[i] = convert_vertex(input_vertices[tri.vertices[i]].vertex);
            }

            const uint32_t base = static_cast<uint32_t>(output_vertices.size());
            const auto normal = calculate_normal(&verts[0]);
            for (int i = 0; i < 3; ++i)
            {
                output_vertices.push_back({ verts[i], normal, Vector2::Zero, platform_and_version.platform == trlevel::Platform::Saturn ? colour_from_texture(tri.texture) : texture_storage.palette_from_texture(texture) });
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
}
