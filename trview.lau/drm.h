#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include <vector>
#include <sstream>
#include <unordered_map>

namespace trview
{
    namespace lau
    {
        struct FileHeader
        {
            uint16_t flags[4];
            uint32_t id;
            uint16_t id_of_next_section;
        };

#pragma pack(push, 1)
        enum class SectionType : uint32_t
        {
            Section = 0,
            Texture = 5,
            Audio = 6
        };

        struct SectionHeader
        {
            uint32_t length;
            SectionType type;
            uint32_t preamble; // extra leading bits 
            uint32_t id;
            uint32_t separator;
        };

        struct Vertex
        {
            int16_t x;
            int16_t y;
            int16_t z;
            int8_t nx;
            int8_t ny;
            int8_t nz;
            uint8_t unknown[3];
            uint16_t u;
            uint16_t v;
            // uint8_t u_1;
            // uint16_t u;
            // uint8_t u_2[5];
            // uint16_t v;
            // int16_t a;
            // int16_t b;
            // int16_t c;
            // int16_t d;
            // // uint16_t d;
            // // float u;
            // // float v;
            // uint8_t u;
            // uint8_t v;
            //uint8_t unknown[10];
        };

#pragma pack(pop)

        struct Section
        {
            uint32_t index;
            SectionHeader header;
            std::vector<uint8_t> data;
            std::vector<std::tuple<uint32_t, uint32_t>> links;

            std::istringstream stream() const
            {
                std::string data(reinterpret_cast<const char*>(&this->data[0]), this->data.size());
                return std::istringstream(data, std::ios::binary);
            }
        };

        struct Vector3
        {
            float x;
            float y;
            float z;
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

        struct Drm
        {
            uint32_t version;
            FileHeader file_header;
            std::vector<Section> sections;
            Vector3 scale;
            std::vector<Vertex> world_mesh;
            std::vector<Triangle> world_triangles;
            std::unordered_map<uint16_t, Texture> textures;
            Vector3 world_offset;
        };

        std::wstring section_type_to_string(const SectionType& type);
    }
}
