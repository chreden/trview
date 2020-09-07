#include "drm.h"
#include <fstream>
#include <vector>

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

        std::unique_ptr<Drm> load_drm(const std::wstring& filename)
        {
            std::ifstream file(filename, std::ios::binary);
            if (!file.is_open())
            {
                return nullptr;
            }

            auto drm = std::make_unique<Drm>();
            drm->version = read<uint16_t>(file);

            uint32_t sections = read<uint32_t>(file);
            std::vector<SectionHeader> headers = read_vector<SectionHeader>(file, sections);

            return drm;
        }
    }
}
