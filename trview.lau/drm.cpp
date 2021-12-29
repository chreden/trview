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

            std::unordered_map<uint32_t, std::unordered_set<uint32_t>> links;

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

            for (const auto& section : drm->sections)
            {
                if (section.header.type == SectionType::Texture)
                {
                    Texture texture;
                    texture.id = section.header.id;
                    auto stream = section.stream();
                    stream.seekg(4, std::ios::cur); // Skip PCD9

                    auto format = read_vector<uint8_t>(stream, 4);
                    texture.format = std::string(format.begin(), format.end());

                    uint32_t pixel_data_length = read<uint32_t>(stream);
                    stream.seekg(4, std::ios::cur); // unknown uint32_t
                    texture.width = read<uint16_t>(stream);
                    texture.height = read<uint16_t>(stream);
                    stream.seekg(4, std::ios::cur); // unknown uint32_t
                    texture.data = read_vector<uint8_t>(stream, pixel_data_length);

                    drm->textures[texture.id] = texture;
                }
            }

            return drm;
        }

        std::wstring section_type_to_string(const SectionType& type)
        {
            switch (type)
            {
            case SectionType::Audio:
                return L"Audio";
            case SectionType::Section:
                return L"Section";
            case SectionType::Texture:
                return L"Texture";
            }
            return L"Unknown";
        }
    }
}
