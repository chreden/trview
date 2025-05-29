#include "trtypes.h"
#include <ranges>

namespace trlevel
{
    namespace
    {
        /// Convert a tr4 object texture into a type that matches 1-3.
        /// @param texture The texture to convert.
        /// @returns The converted texture.
        tr_object_texture convert_object_texture(const tr4_object_texture& texture)
        {
            tr_object_texture new_entity{ texture.Attribute, texture.TileAndFlag };
            memcpy(new_entity.Vertices, texture.Vertices, sizeof(new_entity.Vertices));
            return new_entity;
        }
    }

    uint32_t convert_textile32(uint32_t t)
    {
        uint32_t a = (t & 0xff000000) >> 24;
        uint32_t r = (t & 0x00ff0000) >> 16;
        uint32_t g = (t & 0x0000ff00) >> 8;
        uint32_t b = t & 0x000000ff;
        return a << 24 | b << 16 | g << 8 | r;
    }

    uint32_t convert_textile16(uint16_t t)
    {
        uint16_t r = (t & 0x7c00) >> 10;
        uint16_t g = (t & 0x03e0) >> 5;
        uint16_t b = t & 0x001f;

        r = static_cast<uint16_t>((r / 31.0f) * 255.0f);
        g = static_cast<uint16_t>((g / 31.0f) * 255.0f);
        b = static_cast<uint16_t>((b / 31.0f) * 255.0f);

        uint16_t a = t & 0x8000 ? 0xff : 0x00;
        return a << 24 | b << 16 | g << 8 | r;
    }

    std::vector<uint32_t> convert_textile(const tr_textile16& tile)
    {
        return tile.Tile | std::views::transform(convert_textile16) | std::ranges::to<std::vector>();
    }

    std::vector<uint32_t> convert_textile(const tr_textile32& tile)
    {
        return tile.Tile | std::views::transform(convert_textile32) | std::ranges::to<std::vector>();
    }

    // Convert a set of Tomb Raider I static meshes into a format compatible
    // with Tomb Raider III (what the viewer is currently using).
    std::vector<tr3_room_staticmesh> convert_room_static_meshes(std::vector<tr_room_staticmesh> meshes)
    {
        std::vector<tr3_room_staticmesh> new_meshes;
        new_meshes.reserve(meshes.size());
        std::transform(meshes.begin(), meshes.end(),
            std::back_inserter(new_meshes), [](const auto& mesh)
        {
            tr3_room_staticmesh new_mesh{ mesh.x, mesh.y, mesh.z, mesh.rotation, 0xffff, 0, mesh.mesh_id };
            return new_mesh;
        });
        return new_meshes;
    }

    std::vector<tr3_room_staticmesh> convert_room_static_meshes(std::vector<tr_room_staticmesh_psx> meshes)
    {
        return meshes
            | std::views::transform([](const auto& mesh)
                {
                    tr3_room_staticmesh new_mesh{ mesh.mesh.x, mesh.mesh.y, mesh.mesh.z, mesh.mesh.rotation, 0xffff, 0, mesh.mesh.mesh_id };
                    return new_mesh;
                })
            | std::ranges::to<std::vector>();
    }

    // Convert a set of Tomb Raider I entities into a format compatible
    // with Tomb Raider III (what the viewer is currently using).
    std::vector<tr2_entity> convert_entities(std::vector<tr_entity> entities)
    {
        std::vector<tr2_entity> new_entities;
        new_entities.reserve(entities.size());
        std::transform(entities.begin(), entities.end(),
            std::back_inserter(new_entities), [](const auto& entity)
        {
            tr2_entity new_entity{ entity.TypeID, entity.Room, entity.x, entity.y, entity.z, entity.Angle, entity.Intensity1, entity.Intensity1, entity.Flags };
            return new_entity;
        });
        return new_entities;
    }

    // Convert the tr_colour to a tr_colour4 value.
    tr_colour4 convert_to_colour4(const tr_colour& colour)
    {
        return tr_colour4{static_cast<uint8_t>(colour.Red << 2), static_cast<uint8_t>(colour.Green << 2), static_cast<uint8_t>(colour.Blue << 2), 0x00 };
    }

    // Convert a set of Tomb Raider IV object textures into a format compatible
    // with Tomb Raider III (what the viewer is currently using).
    std::vector<tr_object_texture> convert_object_textures(std::vector<tr4_object_texture> object_textures)
    {
        std::vector<tr_object_texture> new_object_textures;
        new_object_textures.reserve(object_textures.size());
        std::transform(object_textures.begin(), object_textures.end(), std::back_inserter(new_object_textures), convert_object_texture);
        return new_object_textures;
    }

    std::vector<tr_object_texture> convert_object_textures(std::vector<tr5_object_texture> object_textures)
    {
        std::vector<tr_object_texture> new_object_textures;
        new_object_textures.reserve(object_textures.size());
        std::transform(object_textures.begin(), object_textures.end(),
            std::back_inserter(new_object_textures), [](const auto& texture) { return convert_object_texture(texture.tr4_texture); });
        return new_object_textures;
    }

    std::vector<tr4_mesh_face3> convert_triangles(std::vector<tr_face3> triangles)
    {
        std::vector<tr4_mesh_face3> new_triangles;
        new_triangles.reserve(triangles.size());
        std::transform(triangles.begin(), triangles.end(),
            std::back_inserter(new_triangles), [](const auto& tri)
        {
            tr4_mesh_face3 new_face3;
            memcpy(new_face3.vertices, tri.vertices, sizeof(tri.vertices));
            new_face3.texture = tri.texture;
            new_face3.effects = 0;
            return new_face3;
        });
        return new_triangles;
    }

    std::vector<tr4_mesh_face4> convert_rectangles(std::vector<tr_face4> rectangles)
    {
        std::vector<tr4_mesh_face4> new_rectangles;
        new_rectangles.reserve(rectangles.size());
        std::transform(rectangles.begin(), rectangles.end(),
            std::back_inserter(new_rectangles), [](const auto& rect)
        {
            tr4_mesh_face4 new_face4;
            memcpy(new_face4.vertices, rect.vertices, sizeof(rect.vertices));
            new_face4.texture = rect.texture;
            new_face4.effects = 0;
            return new_face4;
        });
        return new_rectangles;
    }

    tr_room_info convert_room_info(const tr1_4_room_info& room_info)
    {
        tr_room_info new_info;
        new_info.x = room_info.x;
        new_info.y = 0;
        new_info.z = room_info.z;
        new_info.yTop = room_info.yTop;
        new_info.yBottom = room_info.yBottom;
        return new_info;
    }

    std::vector<tr_model> convert_models(std::vector<tr_model_psx> models)
    {
        std::vector<tr_model> new_models;
        new_models.reserve(models.size());
        std::transform(models.begin(), models.end(),
            std::back_inserter(new_models), [](const auto& model) { return model.model; });
        return new_models;
    }

    std::vector<tr_model> convert_models(std::vector<tr5_model> models)
    {
        std::vector<tr_model> new_models;
        new_models.reserve(models.size());
        std::transform(models.begin(), models.end(),
            std::back_inserter(new_models), [](const auto& model) { return model.model; });
        return new_models;
    }

    std::vector<tr_vertex> convert_vertices(std::vector<tr_vertex_psx> vertices)
    {
        return vertices
            | std::views::transform([](const auto& vert) { return tr_vertex{ vert.x, vert.y, vert.z }; })
            | std::ranges::to<std::vector>();
    }

    std::vector<tr4_mesh_face3> convert_tr3_psx_room_triangles(std::vector<uint32_t> triangles, uint16_t total_vertices)
    {
        return triangles |
            std::views::transform([=](const auto& t)
                {
                    return tr4_mesh_face3
                    {
                        .vertices =
                        {
                            static_cast<uint16_t>(total_vertices + (t & 0x7f)),
                            static_cast<uint16_t>(total_vertices + ((t >> 7) & 0x7f)),
                            static_cast<uint16_t>(total_vertices + ((t >> 14) & 0x7f))
                        },
                        .texture = static_cast<uint16_t>(t >> 21),
                        .effects = 0
                    };
                }) | std::ranges::to<std::vector>();
    }

    std::vector<tr4_animation> convert_animations(std::vector<tr_animation> animations)
    {
        return animations | std::views::transform([](auto&& a) -> tr4_animation
            {
                return
                {
                    .FrameOffset = a.FrameOffset,
                    .FrameRate = a.FrameRate,
                    .FrameSize = a.FrameSize,
                    .State_ID = a.State_ID,
                    .Speed = a.Speed,
                    .Accel = a.Accel,
                    .SpeedLateral = 0, // TODO: Is this correct?
                    .AccelLateral = 0, // TODO: Is this correct?
                    .FrameStart = a.FrameStart,
                    .FrameEnd = a.FrameEnd,
                    .NextAnimation = a.NextAnimation,
                    .NextFrame = a.NextFrame,
                    .NumStateChanges = a.NumStateChanges,
                    .StateChangeOffset = a.StateChangeOffset,
                    .NumAnimCommands = a.NumAnimCommands,
                    .AnimCommand = a.AnimCommand,
                };
            }) | std::ranges::to<std::vector>();
    }
}