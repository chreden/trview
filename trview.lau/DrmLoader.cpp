#include "DrmLoader.h"
#include <trview.common/Strings.h>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include "Geometry.h"
#include <Windows.h>

namespace trview
{
    namespace lau
    {
        namespace
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

            enum class SectionUsage : uint32_t
            {
                Bounds = 4,
                VertexData = 128
            };

            SectionUsage operator & (SectionUsage left, SectionUsage right)
            {
                using T = std::underlying_type_t<SectionUsage>;
                return static_cast<SectionUsage>(static_cast<T>(left) & static_cast<T>(right));
            }

            bool has_flag(SectionUsage filter, SectionUsage flag)
            {
                return (filter & flag) == flag;
            }

            struct SectionHeaderReference
            {
                uint32_t index;
                SectionUsage usage;
            };
        }

        IDrmLoader::~IDrmLoader()
        {
        }

        DrmLoader::DrmLoader(const std::shared_ptr<IFiles>& files)
            : _files(files)
        {
        }

        std::unique_ptr<Drm> DrmLoader::load(const std::wstring filename) const
        {
            auto data = _files->load_file(to_utf8(filename));
            if (!data.has_value())
            {
                return nullptr;
            }

            auto data_value = data.value();
            std::string file_data(reinterpret_cast<char*>(&data_value[0] ), data_value.size());
            std::istringstream file(file_data, std::ios::binary);

            auto drm = std::make_unique<Drm>();
            drm->version = read<uint32_t>(file);

            uint32_t num_sections = read<uint32_t>(file);
            std::vector<SectionInfo> headers = read_vector<SectionInfo>(file, num_sections);

            uint32_t section_index = 0;

            for (const auto& header : headers)
            {
                const auto section_start = file.tellg();
                std::vector<uint8_t> section_data(header.num_relocations * sizeof(Relocation) + header.length);
                file.read(reinterpret_cast<char*>(&section_data[0]), section_data.size());
                drm->sections.emplace_back(Section{ section_index, header, section_data });
                file.seekg(section_start);
                file.seekg(section_data.size(), std::ios::cur);
                ++section_index;
            }

            // Build dependency links
            for (auto& section : drm->sections)
            {
                auto stream = section.stream();
                section.relocations = read_vector<Relocation>(stream, section.header.num_relocations);
            }

            for (const auto& section : drm->sections)
            {
                std::stringstream stream;
                stream << section.index << ":" << to_utf8(section_type_to_string(section.header.type)) << "-" << section.header.id << '\n';
                OutputDebugStringA(stream.str().c_str());
            }


            read_file_header(*drm);
            generate_textures(*drm);
            return drm;
        }

        void DrmLoader::read_file_header(Drm& drm) const
        {
            if (drm.sections.empty())
            {
                return;
            }

            const auto& header = drm.sections[0];
            auto stream = header.stream();

            auto references = read_vector<Relocation>(stream, header.header.num_relocations);

            // local int start = FTell();

            auto flags = read_vector<uint16_t>(stream, 4);
            uint32_t id = read<uint32_t>(stream);
            bool has_world_mesh = !flags[1];

            if (!flags[1])
            {
                auto world_header = read_vector<uint8_t>(stream, 260);
                float x = read<float>(stream);
                float y = read<float>(stream);
                float z = read<float>(stream);

                // unknown data, ignore for now.
                /*
                uint32 unknown_3;
                ushort unknown_4;
                byte unknown_2[size - (FTell() - start)];
                */
            }

            std::vector<uint32_t> loaded;
            for (const auto& reference : references)
            {
                // uint32_t index = reference.index >> 3;
                if (std::find(loaded.begin(), loaded.end(), reference.section_index_or_type) != loaded.end())
                {
                    continue;
                }

                loaded.push_back(reference.section_index_or_type);

                // Temporarily disabled while reolcations work
                /*
                if (!has_world_mesh && has_flag(reference.usage, SectionUsage::VertexData))
                {
                    if (!has_flag(reference.usage, SectionUsage::Bounds))
                    {
                        load_vertex_data(drm, drm.sections[index]);
                    }
                }
                */
            }

            if (has_world_mesh)
            {
                // This seems to be true, mostly - the biggest generic section tends to be
                // the vertices of the world mesh.
                uint32_t world_mesh_index = 0;
                for (uint32_t i = 1; i < drm.sections.size(); ++i)
                {
                    if (drm.sections[i].header.type == SectionType::Section)
                    {
                        world_mesh_index = i;
                        break;
                    }
                }

                // The mesh index seems to be the first reference in the file header (so far).pleas
                uint32_t mesh_index = references[0].section_index_or_type;

                // First header of the file header is the mesh (section 11).
                load_world_vertices(drm, drm.sections[world_mesh_index]);
                load_world_mesh(drm, drm.sections[8]);
                load_world_mesh(drm, drm.sections[12]);
                load_world_mesh(drm, drm.sections[mesh_index]);
                resolve_world_mesh_textures(drm, drm.sections[mesh_index]);

                // eg18
                // load_world_vertices(drm, drm.sections[4]);
                // load_world_mesh(drm, drm.sections[5]);
                // load_world_mesh(drm, drm.sections[12]);
                // load_world_mesh(drm, drm.sections[11]);
                // resolve_world_mesh_textures(drm, drm.sections[11]);
            }
        }

        void DrmLoader::generate_textures(Drm& drm) const
        {
            for (const auto& section : drm.sections)
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

                    drm.textures[texture.id] = texture;
                }
            }
        }

        void DrmLoader::load_vertex_data(Drm& drm, const Section& section) const
        {
            auto stream = section.stream();
            // uint32_t number_of_headers = section.header.preamble / 32 / 8;
            auto references = read_vector<Relocation>(stream, section.header.num_relocations);

            auto start = stream.tellg();

            read_vector<uint16_t>(stream, 8);

            float scale_x = read<float>(stream);
            float scale_y = read<float>(stream);
            float scale_z = read<float>(stream);
            drm.scale = { scale_x, scale_y, scale_z };
            float scale_w = read<float>(stream);

            uint32_t vertex_count = read<uint32_t>(stream);
            uint32_t length = read<uint32_t>(stream);

            read_vector<uint8_t>(stream, 20);

            Vector3 offset = read<Vector3>(stream);

            read_vector<uint8_t>(stream, length - 40 - 20 - 12);

            uint16_t base = static_cast<uint16_t>(drm.world_mesh.size());

            auto vertices = read_vector<MVertex>(stream, vertex_count);
            for (auto& vert : vertices)
            {
                auto uv = convert_uv(vert.uv);
                drm.world_mesh.push_back(
                    Vertex
                    {
                        Vector3 { static_cast<float>(vert.x), static_cast<float>(vert.y), static_cast<float>(vert.z) },
                        Vector3 { static_cast<float>(vert.nx), static_cast<float>(vert.ny), static_cast<float>(vert.nz) },
                        Vector2 { uv.x, uv.y }
                    });
                // vert.x += offset.x;
                // vert.y += offset.y;
                // vert.z += offset.z;
            }

            bool more_mesh = false;
            do
            {
                // Load the mesh:
                uint16_t index_count = read<uint16_t>(stream);
                skip(stream, 2);
                uint16_t texture_id = read<uint16_t>(stream);
                read_vector<uint16_t>(stream, 5);
                uint32_t eom = read<uint32_t>(stream);
                auto indices = read_vector<uint16_t>(stream, index_count);

                for (uint32_t i = 0; i < indices.size() / 3; ++i)
                {
                    drm.world_triangles.push_back(
                        {
                            static_cast<uint32_t>(indices[i * 3] + base),
                            static_cast<uint32_t>(indices[i * 3 + 1] + base),
                            static_cast<uint32_t>(indices[i * 3 + 2] + base),
                            texture_id
                        });
                }

                stream.seekg(start, std::ios::beg);
                stream.seekg(eom, std::ios::cur);

                more_mesh = read<uint32_t>(stream) > 0;
                if (more_mesh)
                {
                    stream.seekg(-4, std::ios::cur);
                }

            } while (more_mesh);
        }

        void DrmLoader::load_world_vertices(Drm& drm, const Section& section) const
        {
            drm.scale = { 0.1, 0.1, 0.1 };

            const auto& world_mesh_section = drm.sections[1];
            auto world_stream = world_mesh_section.stream();
            const auto length = world_mesh_section.data.size() / sizeof(WorldVertex);
            const auto world_vertex = read_vector<WorldVertex>(world_stream, length);
            const auto base = drm.world_mesh.size();
            std::stringstream indices_output;

            for (const auto& vertex : world_vertex)
            {
                const float factor = 4096.0f;
                float u = vertex.u / factor;
                float v = vertex.v / factor;
                drm.world_mesh.push_back(
                    Vertex
                    {
                        Vector3 { static_cast<float>(vertex.x), static_cast<float>(vertex.y), static_cast<float>(vertex.z) },
                        Vector3 { -static_cast<float>(vertex.x), -static_cast<float>(vertex.y), -static_cast<float>(vertex.z) },
                        Vector2 { u, v }
                    });
            }
        }

        void DrmLoader::load_world_mesh(Drm& drm, const Section& section) const
        {
            auto stream = section.stream();
            // uint32_t number_of_headers = section.header.preamble / 32 / 8;
            auto references = read_vector<Relocation>(stream, section.header.num_relocations);

            const uint32_t start = stream.tellg();

            if (section.index == 12)
            {
                // This is fine.
                stream.seekg(1904, std::ios::cur);
            }

            uint32_t num = 0;
            bool done = false;
            while (!done)
            {
                uint32_t num_indices = read<uint32_t>(stream);
                if (num_indices == 0)
                {
                    break;
                }

                // Rest of header, whatever it is.
                stream.seekg(16, std::ios::cur);
                uint16_t texture = read<uint16_t>(stream);

                stream.seekg(18, std::ios::cur);
                auto where = static_cast<std::size_t>(stream.tellg()) - start;
                uint32_t end = read<uint32_t>(stream);

                if (end == 0)
                {
                    break;
                }

                auto indices = read_vector<uint16_t>(stream, num_indices);
                const auto tris = num_indices / 3;
                for (uint16_t i = 0; i < tris; ++i)
                {
                    drm.world_triangles.push_back(
                        Triangle
                        {
                            static_cast<uint32_t>(indices[i * 3]),
                            static_cast<uint32_t>(indices[i * 3 + 1]),
                            static_cast<uint32_t>(indices[i * 3 + 2]),
                            texture
                        });
                }

                stream.seekg(start + end, std::ios::beg);

                uint16_t value = read<uint16_t>(stream);
                while (value == 0)
                {
                    if (stream.tellg() >= section.data.size())
                    {
                        done = true;
                        break;
                    }
                    value = read<uint16_t>(stream);
                }
                stream.seekg(-2, std::ios::cur);
            }
        }

        void DrmLoader::resolve_world_mesh_textures(Drm& drm, const Section& section) const
        {
            std::unordered_set<uint16_t> texture_indices;
            for (const auto& tri : drm.world_triangles)
            {
                texture_indices.insert(tri.texture);
            }

            auto stream = section.stream();
            stream.seekg(section.data.size() - 738, std::ios::beg);
            stream.seekg(18, std::ios::cur);

            std::vector<uint16_t> textures;
            for (int i = 0; i < texture_indices.size(); ++i)
            {
                textures.push_back(read<uint16_t>(stream));
                stream.seekg(18, std::ios::cur);
            }

            for (uint32_t i = 0; i < drm.world_triangles.size(); ++i)
            {
                drm.world_triangles[i].texture = textures[drm.world_triangles[i].texture];
            }
        }
    }
}
