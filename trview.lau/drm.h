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
        enum class SectionType : uint8_t
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

        struct SectionInfo
        {
            int32_t length;
            SectionType type;
            int8_t skip : 1;
            int8_t __free : 7;
            uint16_t version_id;
            uint32_t has_debug_info : 1;
            uint32_t resource_type : 7;
            uint32_t num_relocations : 24;
            int32_t id;
            uint32_t spec_mask;
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

        struct WorldVertex
        {
            int16_t x;
            int16_t y;
            int16_t z;
            int16_t w;
            int32_t colour;
            int16_t u;
            int16_t v;
            uint16_t bend;
            uint16_t bend_index;
        };

#pragma pack(pop)

        struct Relocation
        {
            uint16_t type : 3;
            uint16_t section_index_or_type : 13;
            uint16_t type_specific;
            uint32_t offset;
        };

        struct Section
        {
            uint32_t index;
            SectionInfo header;
            std::vector<uint8_t> data;
            std::vector<Relocation> relocations;

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

        struct Vector2
        {
            float x;
            float y;
        };

        struct Triangle
        {
            uint32_t v0, v1, v2;
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

        struct Vertex
        {
            Vector3 position;
            Vector3 normal;
            Vector2 uv;
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
