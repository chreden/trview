#include "drm.h"
#include <fstream>
#include <unordered_set>

namespace trview
{
    namespace lau
    {
        namespace
        {
            template < typename T >
            void read(std::istream& file, T& value)
            {
                file.read(reinterpret_cast<char*>(&value), sizeof(value));
            }

            template <typename T>
            T read(std::istream& file)
            {
                T value;
                read<T>(file, value);
                return value;
            }

            template < typename DataType, typename SizeType >
            std::vector<DataType> read_vector(std::istream& file, SizeType size)
            {
                std::vector<DataType> data(size);
                for (SizeType i = 0; i < size; ++i)
                {
                    read<DataType>(file, data[i]);
                }
                return data;
            }

            void skip(std::istream& file, uint32_t size)
            {
                file.seekg(size, std::ios::cur);
            }

            std::vector<std::tuple<uint32_t, uint32_t>> get_links(const Section& section, std::istream& stream)
            {
                std::vector<std::tuple<uint32_t, uint32_t>> links;
                for (auto i = 0u; i < section.header.preamble / 32 / 8; ++i)
                {
                    uint32_t referenced_section = read<uint32_t>(stream) >> 3;
                    uint32_t value = read<uint32_t>(stream);
                    links.push_back({ referenced_section, value });
                }
                return links;
            }

            std::vector<uint32_t> get_filtered_links(const Section& section, const std::vector<std::tuple<uint32_t, uint32_t>>& links, std::unordered_set<uint32_t>& visited_sections)
            {
                std::vector<uint32_t> filtered_links;
                for (const auto& link : links)
                {
                    auto index = std::get<0>(link);
                    if (index == 0 || visited_sections.find(index) != visited_sections.end() ||
                        std::find(filtered_links.begin(), filtered_links.end(), index) != filtered_links.end())
                    {
                        continue;
                    }
                    filtered_links.push_back(index);
                }
                return filtered_links;
            }

            void read_file_header(Drm& drm, std::vector<Section>& sections)
            {
                auto& file_manifest_section = sections[0];
                auto file_manifest = file_manifest_section.stream();
                file_manifest_section.links = get_links(file_manifest_section, file_manifest);

                for (int i = 0; i < 4; ++i)
                {
                    drm.file_header.flags[i] = read<uint16_t>(file_manifest);
                }
                drm.file_header.id = read<uint32_t>(file_manifest);
            }
        }

        std::unique_ptr<Drm> load_drm(const std::wstring& filename)
        {
            std::ifstream file(filename, std::ios::binary);
            if (!file.is_open())
            {
                return nullptr;
            }

            auto drm = std::make_unique<Drm>();
            drm->version = read<uint32_t>(file);

            uint32_t num_sections = read<uint32_t>(file);
            std::vector<SectionHeader> headers = read_vector<SectionHeader>(file, num_sections);

            uint32_t section_index = 0;
            for (const auto& header : headers)
            {
                const auto section_start = file.tellg();
                std::vector<uint8_t> section_data(header.preamble / 32 + header.length);
                file.read(reinterpret_cast<char*>(&section_data[0]), section_data.size());
                drm->sections.emplace_back(Section{ section_index, header, section_data });
                file.seekg(section_start);
                file.seekg(section_data.size(), std::ios::cur);
                ++section_index;
            }

            // Hopefully this never happens.
            if (drm->sections.empty())
            {
                return drm;
            }

            read_file_header(*drm, drm->sections);
            return drm;
        }
    }
}
