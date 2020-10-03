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

            void read_meshes(Drm& drm, const std::vector<Section>& sections, const Section& section, std::unordered_set<uint32_t>& visited_sections, int32_t depth)
            {
                visited_sections.insert(section.index);

                // Parse the world manifest section (assuming that's what this is....)
                auto stream = section.stream();
                auto links = get_links(section, stream);
                auto filtered_links = get_filtered_links(section, links, visited_sections);

                if (!filtered_links.empty())
                {
                    uint32_t skip = depth == 1 ? 1 : 0;
                    for (auto i = 0u; i < filtered_links.size() - skip; ++i)
                    {
                        if (sections[filtered_links[i]].header.type == SectionType::Section)
                        {
                            read_meshes(drm, sections, sections[filtered_links[i]], visited_sections, depth + 1);
                        }
                    }

                    if (depth == 1)
                    {
                        const auto world_mesh_index = filtered_links.back();
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
                while (!separator_next(stream) && stream.tellg() < section.data.size() - 8)
                {
                    stream.seekg(4, std::ios::cur);
                }

                auto at = stream.tellg();
                std::streampos next_start;
                std::vector<std::vector<uint16_t>> all_entries;
                bool has_entries = false;
                while (separator_next(stream) && stream.tellg() < section.data.size() - 8)
                {
                    uint32_t num_entries = read<uint32_t>(stream);
                    uint32_t separator = read<uint32_t>(stream);
                    auto unknown = read_vector<uint32_t>(stream, 8);
                    uint32_t nsv = read<uint32_t>(stream);
                    next_start = std::streampos(nsv);
                    auto entries = read_vector<uint16_t>(stream, num_entries);
                    all_entries.push_back(entries);
                    has_entries = has_entries | !entries.empty();
                    if (stream.tellg() < start + next_start)
                    {
                        stream.seekg(start + next_start, std::ios::beg);
                        auto at = stream.tellg();
                    }
                    while (is_zero(stream) && stream.tellg() < section.data.size() - 4)
                    {
                    }
                    at = stream.tellg();
                }

                // TODO: Read texture information
                if (!all_entries.empty())
                {
                    // Read indices:
                    auto sum = 0;
                    for (const auto& s : all_entries) 
                    {
                        sum += s.size();
                    };

                    // Hack for reading mesh data:
                    if (has_entries && stream.tellg() < section.data.size())
                    {
                        auto x = stream.tellg();
                        
                        std::vector<std::vector<uint16_t>> texture_entry_groups;

                        auto more_entry_groups = [](std::istream& stream)
                        {
                            stream.seekg(2, std::ios::cur);
                            const auto separator = read<uint16_t>(stream);
                            stream.seekg(-4, std::ios::cur);
                            return (separator & 32) == 0;
                        };

                        while (true)
                        {
                            std::vector<uint16_t> current_group;
                            while (true)
                            {
                                uint16_t value = read<uint16_t>(stream);
                                if (value == 0)
                                {
                                    break;
                                }
                                current_group.push_back(value);
                            }

                            texture_entry_groups.push_back(current_group);
                            if (!more_entry_groups(stream))
                            {
                                break;
                            }
                        }

                        struct TextureEntry
                        {
                            uint8_t flags;
                            uint16_t texture_id;
                        };

                        std::vector<TextureEntry> all_texture_entries;
                        for (const auto& group : texture_entry_groups)
                        {
                            for (int i = 0; i < group.size(); ++i)
                            {
                                TextureEntry new_entry;
                                new_entry.texture_id = read<uint16_t>(stream);
                                new_entry.flags = (new_entry.texture_id & 0xf000) >> 12;
                                new_entry.texture_id = new_entry.texture_id & 0xfff;

                                stream.seekg(18, std::ios::cur);

                                all_texture_entries.push_back(new_entry);
                            }
                        }

                        stream.seekg(data_start + std::streampos(section.header.length - 100), std::ios::beg);
                        uint16_t more_meshes_section = read<uint16_t>(stream);
                        if (more_meshes_section < sections.size() &&
                            sections[more_meshes_section].header.type == SectionType::Section &&
                            visited_sections.find(more_meshes_section) == visited_sections.end())
                        {
                            read_meshes(drm, sections, sections[more_meshes_section], visited_sections, depth + 1);
                        }
                    }

                    // Convert these entries to triangles...
                    for (const auto& mesh : all_entries)
                    {
                        for (auto i = 0u; i < mesh.size(); i += 3)
                        {
                            drm.world_triangles.push_back({ mesh[i], mesh[i + 1u], mesh[i + 2u] });
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
                std::unordered_set<uint32_t> visited_sections{ world_manifest_section.index, 0 };
                read_meshes(*drm, sections, world_manifest_section, visited_sections, 1);
            }

            // Load textures
            for (const auto& section : sections)
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
    }
}
