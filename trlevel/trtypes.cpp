#include "trtypes.h"
#include <algorithm>
#include <iterator>

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

        r <<= 3;
        g <<= 3;
        b <<= 3;

        r += 3;
        g += 3;
        b += 3;

        uint16_t a = t & 0x8000 ? 0xff : 0x00;
        return a << 24 | b << 16 | g << 8 | r;
    }

    // Convert a set of Tomb Raider I vertices into a vertex format compatible
    // with Tomb Raider III (what the viewer is currently using).
    std::vector<tr3_room_vertex> convert_vertices(std::vector<tr_room_vertex> vertices)
    {
        std::vector<tr3_room_vertex> new_vertices;
        new_vertices.reserve(vertices.size());
        std::transform(vertices.begin(), vertices.end(),
            std::back_inserter(new_vertices), [](const auto& vert)
        {
            tr3_room_vertex new_vertex { vert.vertex, vert.lighting, 0, 0xffff };
            return new_vertex;
        });
        return new_vertices;
    }

    std::vector<tr3_room_vertex> convert_vertices(std::vector<tr5_room_vertex> vertices)
    {
        std::vector<tr3_room_vertex> new_vertices;
        new_vertices.reserve(vertices.size());
        std::transform(vertices.begin(), vertices.end(),
            std::back_inserter(new_vertices), [](const auto& vert)
        {
            tr_vertex vertex{ static_cast<int16_t>(vert.vertex.x), static_cast<int16_t>(vert.vertex.y), static_cast<int16_t>(vert.vertex.z) };
            return tr3_room_vertex { vertex, 0, 0, 0xffff };
        });
        return new_vertices;
    }

    // Convert a set of Tomb Raider I lights into a light format compatible
    // with Tomb Raider III (what the viewer is currently using).
    std::vector<tr3_room_light> convert_lights(std::vector<tr_room_light> lights)
    {
        std::vector<tr3_room_light> new_lights;
        new_lights.reserve(lights.size());
        std::transform(lights.begin(), lights.end(),
            std::back_inserter(new_lights), [](const auto& light)
        {
            tr3_room_light new_light { light.x, light.y, light.z, tr_colour4 { 0xff, 0xff, 0xff, 0xff }, light.intensity, light.fade };
            return new_light;
        });
        return new_lights;
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

    std::vector<tr_model> convert_models(std::vector<tr5_model> models)
    {
        std::vector<tr_model> new_models;
        new_models.reserve(models.size());
        std::transform(models.begin(), models.end(),
            std::back_inserter(new_models), [](const auto& model) { return model.model; });
        return new_models;
    }
}