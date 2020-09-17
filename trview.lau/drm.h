#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>

namespace trview
{
    namespace lau
    {
        struct Vector3
        {
            float x;
            float y;
            float z;
        };

        struct Vertex
        {
            int16_t x;
            int16_t y;
            int16_t z;
            uint16_t a;
            uint16_t b;
            uint16_t c;
            uint16_t d;
            uint16_t e;
            uint16_t f;
            uint16_t g;
        };

        struct Triangle
        {
            uint16_t v0, v1, v2;
        };

        struct Texture
        {
            uint16_t id{ 0u };
            std::string format;
            uint16_t width{ 0u };
            uint16_t height{ 0u };
            std::vector<uint8_t> data;
        };

        struct FileHeader
        {
            uint16_t flags[4];
            uint32_t id;
            uint16_t id_of_next_section;
        };

        struct Drm
        {
            uint32_t version;
            FileHeader file_header;
            std::vector<Vertex> world_mesh;
            std::vector<Triangle> world_triangles;
            std::unordered_map<uint16_t, Texture> textures;
            Vector3 world_offset;
        };

        std::unique_ptr<Drm> load_drm(const std::wstring& filename);
    }
}
