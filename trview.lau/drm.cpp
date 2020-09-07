#include "drm.h"
#include <fstream>

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

            struct FileHeader
            {
                uint16_t flags[4];
                uint16_t id;
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

            FileHeader file_header;

            uint32_t zero_sections = 0;
            for (const auto& header : headers)
            {
                switch (header.type)
                {
                    case SectionType::Section:
                    {
                        if (zero_sections == 0)
                        {
                            skip(file, header.preamble / 32);
                            file_header = read<FileHeader>(file);
                            skip(file, header.length - 10);
                        }
                        else if (zero_sections == 1)
                        {
                            if (file_header.flags[1])
                            {
                                // Vertex data - skip for now.
                                skip(file, header.preamble / 32);
                                skip(file, header.length);
                            }
                            else
                            {
                                const auto num_vertices = header.length / 20;
                                for (uint32_t i = 0; i < num_vertices; ++i)
                                {
                                    drm->world_mesh.push_back(read<Vertex>(file));
                                    skip(file, 14);
                                }
                            }
                        }
                        else
                        {
                            skip(file, header.preamble / 32);
                            skip(file, header.length);
                        }
                        ++zero_sections;
                        break;
                    }
                    default:
                    {
                        skip(file, header.preamble / 32);
                        skip(file, header.length);
                        break;
                    }
                }
            }

            return drm;
        }
    }
}
