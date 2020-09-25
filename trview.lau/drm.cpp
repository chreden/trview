#include "drm.h"
#include <fstream>
#include <unordered_set>
#include <sstream>

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

            std::vector<std::tuple<uint32_t, uint32_t>> get_links(const Section& section, std::istream& stream)
            {
                std::vector<std::tuple<uint32_t, uint32_t>> links;
                for (auto i = 0; i < section.header.preamble / 32 / 8; ++i)
                {
                    uint32_t referenced_section = read<uint32_t>(stream) >> 3;
                    uint32_t value = read<uint32_t>(stream);
                    links.push_back({ referenced_section, value });
                }
                return links;
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

                if (!drm.file_header.flags[1])
                {
                    // Room:
                    skip(file_manifest, 262);
                    drm.world_offset = read<Vector3>(file_manifest);
                    skip(file_manifest, 6);
                    skip(file_manifest, file_manifest_section.header.length - 290);
                }
                else
                {
                    skip(file_manifest, 24);
                    uint32_t size_of_section = read<uint32_t>(file_manifest);
                    skip(file_manifest, size_of_section - 32 - 10);
                    drm.file_header.id_of_next_section = read<uint16_t>(file_manifest);
                    skip(file_manifest, 6);
                }
            }

            const Section& find_section(const std::vector<Section>& sections, uint32_t id)
            {
                return *std::find_if(sections.begin(), sections.end(),
                    [id](const Section& section) { return section.header.id == id; });
            }

            void read_meshes(Drm& drm, const std::vector<Section>& sections, const Section& section)
            {
                // Parse the world manifest section (assuming that's what this is....)
                auto stream = section.stream();
                auto links = get_links(section, stream);

                auto data_start = stream.tellg();

                auto separator_next = [](std::istream& stream)
                {
                    stream.seekg(4, std::ios::cur);
                    const auto separator = read<uint32_t>(stream);
                    stream.seekg(-8, std::ios::cur);
                    return separator == 0xffffffff;
                };

                auto is_zero = [](std::istream& stream)
                {
                    auto zero = read<uint32_t>(stream) == 0;
                    if (!zero)
                    {
                        stream.seekg(-4, std::ios::cur);
                    }
                    return zero;
                };

                const auto start = stream.tellg();

                if (!separator_next(stream))
                {
                    // This is the case where the list of entries (whatever they are) isn't first.
                    stream.seekg(
                        section.index == 12 ? 1904 : 32, 
                        std::ios::cur);
                }

                std::vector<std::vector<uint16_t>> all_entries;
                while (separator_next(stream))
                {
                    uint32_t num_entries = read<uint32_t>(stream);
                    uint32_t separator = read<uint32_t>(stream);
                    auto unknown = read_vector<uint32_t>(stream, 8);
                    auto next_start = static_cast<std::streampos>(read<uint32_t>(stream));
                    auto entries = read_vector<uint16_t>(stream, num_entries);
                    all_entries.push_back(entries);
                    if (stream.tellg() < start + next_start)
                    {
                        stream.seekg(start + next_start, std::ios::beg);
                        auto at = stream.tellg();
                    }
                    while (is_zero(stream) && stream.tellg() < section.data.size() - 4)
                    {
                    }
                    auto at = stream.tellg();
                }

                // Recruit more indices... more meshes.
                stream.seekg(data_start + std::streampos(section.header.length - 100), std::ios::beg);
                uint16_t more_meshes_section = read<uint16_t>(stream);
                if (more_meshes_section < sections.size())
                {
                    read_meshes(drm, sections, sections[more_meshes_section]);
                }
                else if (section.index != 12)
                {
                    read_meshes(drm, sections, sections[12]);
                }

                // Convert these entries to triangles...
                for (const auto& mesh : all_entries)
                {
                    for (auto i = 0u; i < mesh.size(); i += 3)
                    {
                        drm.world_triangles.push_back({ mesh[i], mesh[i + 1u], mesh[i + 2u] });
                    }
                }

                // Find the vertices:
                if (links.size() > 3)
                {
                    const auto& verts_pointer = links[links.size() - 3];
                    const auto world_mesh_index = std::get<0>(verts_pointer);
                    if (world_mesh_index != section.index)
                    {
                        const auto& world_mesh = sections[world_mesh_index];
                        auto world_mesh_stream = world_mesh.stream();
                        const auto num_vertices = world_mesh.header.length / 20;
                        for (uint32_t i = 0; i < num_vertices; ++i)
                        {
                            drm.world_mesh.push_back(read<Vertex>(world_mesh_stream));
                        }
                    }
                }
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

            uint32_t zero_sections = 0;

            uint32_t section_index = 0;

            std::unordered_map<uint32_t, std::unordered_set<uint32_t>> links;

            std::vector<Section> sections;

            for (const auto& header : headers)
            {
                const auto section_start = file.tellg();
                std::vector<uint8_t> section_data(header.preamble / 32 + header.length);
                file.read(reinterpret_cast<char*>(&section_data[0]), section_data.size());
                sections.emplace_back(Section{ section_index, header, section_data });
                file.seekg(section_start);
                file.seekg(section_data.size(), std::ios::cur);
#if 0

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
#else
                ++section_index;
            }

            // Hopefully this never happens.
            if (sections.empty())
            {
                return drm;
            }

            read_file_header(*drm, sections);

            if (drm->file_header.flags[1])
            {
                // Mesh
                auto next_section = find_section(sections, drm->file_header.id_of_next_section);
            }
            else
            {
                // Load a level file - this is all strictly not to do with the DRM file itself - it should
                // really be done outside but can be moved later.
                const auto& world_manifest_section = sections[std::get<0>(sections[0].links[0])];
                read_meshes(*drm, sections, world_manifest_section);
            }
#endif

            return drm;
        }

        
    }
}
