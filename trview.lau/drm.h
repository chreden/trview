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
            Empty = 1,
            Animation = 2,
            PushBuffer_Wc = 3,
            PushBuffer = 4,
            Texture = 5,
            Audio = 6,
            DtpData = 7,
            Script = 8,
            ShaderLib = 9,
            CollisionMesh = 10
        };

        struct SectionHeader
        {
            uint32_t length;
            SectionType type;
            uint32_t preamble; // extra leading bits 
            uint32_t id;
            uint32_t separator;
        };

        struct MVertex
        {
            int16_t x;
            int16_t y;
            int16_t z;
            int8_t nx;
            int8_t ny;
            int8_t nz;
            uint8_t pad;
            uint16_t segment;
            int32_t uv;
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
            uint16_t texture;
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
            std::vector<MVertex> world_mesh;
            std::vector<Triangle> world_triangles;
            std::unordered_map<uint16_t, Texture> textures;
            Vector3 world_offset;
        };

        std::wstring section_type_to_string(const SectionType& type);
    }
}
