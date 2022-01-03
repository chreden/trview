#include "DrmLoader.h"
#include <trview.common/Strings.h>
#include <unordered_map>
#include <unordered_set>

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

            uint32_t number_of_headers = header.header.preamble / 32 / 8;
            auto references = read_vector<SectionHeaderReference>(stream, number_of_headers);

            for (const auto& reference : references)
            {
                if (has_flag(reference.usage, SectionUsage::VertexData))
                {
                    if (!has_flag(reference.usage, SectionUsage::Bounds))
                    {
                        load_vertex_data(drm, drm.sections[reference.index >> 3]);
                    }
                }
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
            uint32_t number_of_headers = section.header.preamble / 32 / 8;
            auto references = read_vector<SectionHeaderReference>(stream, number_of_headers);

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

            auto vertices = read_vector<Vertex>(stream, vertex_count);
            for (auto& vert : vertices)
            {
                // Temporarily remove this part
                // vert.x += offset.x;
                // vert.y += offset.y;
                // vert.z += offset.z;
            }
            drm.world_mesh.insert(drm.world_mesh.end(), vertices.begin(), vertices.end());

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
                            static_cast<uint16_t>(indices[i * 3] + base),
                            static_cast<uint16_t>(indices[i * 3 + 1] + base),
                            static_cast<uint16_t>(indices[i * 3 + 2] + base),
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
    }
}
