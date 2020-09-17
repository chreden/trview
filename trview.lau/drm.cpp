#include "drm.h"
#include <fstream>
#include <unordered_set>

namespace trview
{
    namespace lau
    {
        namespace
        {
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

            struct WorldMeshHeader
            {
                uint32_t type;
                uint32_t start;
            };
#pragma pack(pop)
        }

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

        std::unique_ptr<Drm> load_drm(const std::wstring& filename)
        {
            std::ifstream file(filename, std::ios::binary);
            if (!file.is_open())
            {
                return nullptr;
            }

            auto drm = std::make_unique<Drm>();
            drm->version = read<uint32_t>(file);

            uint32_t sections = read<uint32_t>(file);
            std::vector<SectionHeader> headers = read_vector<SectionHeader>(file, sections);

            uint32_t zero_sections = 0;

            uint32_t section_index = 0;

            std::unordered_map<uint32_t, std::unordered_set<uint32_t>> links;

            for (const auto& header : headers)
            {
                // Read the header for the section if there is one.
                if (header.preamble > 0)
                {
                    auto number_of_headers = (header.preamble / 32) / 8;
                    for (auto i = 0; i < number_of_headers; ++i)
                    {
                        uint32_t referenced_section = read<uint32_t>(file) >> 3;
                        uint32_t value = read<uint32_t>(file);
                        links[section_index].insert(referenced_section);
                    }
                }

                switch (header.type)
                {
                    case SectionType::Section:
                    {
                        if (zero_sections == 0)
                        {
                            for (int i = 0; i < 4; ++i)
                            {
                                drm->file_header.flags[i] = read<uint16_t>(file);
                            }
                            drm->file_header.id = read<uint32_t>(file);

                            if (!drm->file_header.flags[1])
                            {
                                // Room:
                                skip(file, 262);
                                drm->world_offset = read<Vector3>(file);
                                skip(file, 6);
                                skip(file, header.length - 290);
                            }
                            else
                            {
                                skip(file, 120);
                                drm->file_header.id_of_next_section = read<uint16_t>(file);
                                skip(file, 6);
                            }
                        }
                        else if (zero_sections == 1)
                        {
                            if (drm->file_header.flags[1])
                            {
                                // Vertex data - skip for now.
                                skip(file, header.length);
                            }
                            else
                            {
                                const auto num_vertices = header.length / 20;
                                for (uint32_t i = 0; i < num_vertices; ++i)
                                {
                                    drm->world_mesh.push_back(read<Vertex>(file));
                                }
                            }
                        }
                        else
                        {
                            uint32_t type = read<uint32_t>(file);
                            // Disabled for now.
                            if (type == -1)
                            {
                                file.seekg(-4, std::ios::cur);
                                // Read world model data:
                                const uint32_t number_of_headers = (header.preamble / 32) / 8;
                                auto world_mesh_headers = read_vector<WorldMeshHeader>(file, number_of_headers);

                                auto start = file.tellg();
                                for (auto i = 0; i < number_of_headers; ++i)
                                {
                                    file.seekg(start + static_cast<std::streamoff>(world_mesh_headers[i].start - 40), std::ios::beg);

                                    uint32_t number_of_indices = read<uint32_t>(file);
                                    uint32_t separator = read<uint32_t>(file);
                                    if (separator != 0xffffffff)
                                    {
                                        continue;
                                    }

                                    skip(file, 32);
                                    uint32_t end_of_mesh = read<uint32_t>(file);
                                    auto indices = read_vector<uint16_t>(file, number_of_indices);
                                    for (auto j = 0u; j < indices.size(); j += 3)
                                    {
                                        drm->world_triangles.push_back({ indices[j], indices[j + 1u], indices[j + 2u] });
                                    }
                                }

                                file.seekg(start + static_cast<std::streamoff>(header.length), std::ios::beg);
                            }
                            else
                            {
                                file.seekg(-4, std::ios::cur);
                                skip(file, header.length);
                            }
                        }
                        ++zero_sections;
                        break;
                    }
                    case SectionType::Texture:
                    {
                        Texture texture;
                        texture.id = header.id;
                        skip(file, 4); // Skip PCD9

                        auto format = read_vector<uint8_t>(file, 4);
                        texture.format = std::string(format.begin(), format.end());

                        uint32_t pixel_data_length = read<uint32_t>(file);
                        skip(file, 4); // unknown uint32_t
                        texture.width = read<uint16_t>(file);
                        texture.height = read<uint16_t>(file);
                        skip(file, 4); // unknown uint32_t
                        texture.data = read_vector<uint8_t>(file, pixel_data_length);

                        drm->textures[texture.id] = texture;
                        break;
                    }
                    default:
                    {
                        skip(file, header.length);
                        break;
                    }
                }
                ++section_index;
            }

            return drm;
        }
    }
}
