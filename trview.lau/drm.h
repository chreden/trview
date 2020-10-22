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
            Audio = 6,
            Trigger = 7,
            WorldMesh = 10
        };

        struct SectionHeader
        {
            uint32_t length;
            SectionType type;
            uint32_t preamble; // extra leading bits 
            uint32_t id;
            uint32_t separator;
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
                std::string data(reinterpret_cast<const char*>(&data[0]), data.size());
                return std::istringstream(data, std::ios::binary);
            }
        };

        struct Drm
        {
            uint32_t version;
            FileHeader file_header;
            std::vector<Section> sections;
        };

        std::unique_ptr<Drm> load_drm(const std::wstring& filename);
    }
}
