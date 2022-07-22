#include "Level.h"
#include "LevelLoadException.h"
#include "LevelEncryptedException.h"
#include <format>

namespace trlevel
{
    namespace
    {
        const float PiMul2 = 6.283185307179586476925286766559f;
        const int16_t Lara = 0;
        const int16_t LaraSkinTR3 = 315;
        const int16_t LaraSkinPostTR3 = 8;
    }

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

        template < typename SizeType, typename DataType >
        std::vector<DataType> read_vector(std::istream& file)
        {
            auto size = read<SizeType>(file);
            return read_vector<DataType, SizeType>(file, size);
        }

        std::vector<uint8_t> read_compressed(std::istream& file)
        {
            auto uncompressed_size = read<uint32_t>(file);
            auto compressed_size = read<uint32_t>(file);
            auto compressed = read_vector<uint8_t>(file, compressed_size);
            std::vector<uint8_t> uncompressed_data(uncompressed_size);

            z_stream stream;
            memset(&stream, 0, sizeof(stream));
            int result = inflateInit(&stream);
            // Exception...
            stream.avail_in = compressed_size;
            stream.next_in = &compressed[0];
            stream.avail_out = uncompressed_size;
            stream.next_out = &uncompressed_data[0];
            result = inflate(&stream, Z_NO_FLUSH);
            inflateEnd(&stream);

            return uncompressed_data;
        }

        template < typename DataType >
        std::vector<DataType> read_vector_compressed(std::istream& file, uint32_t elements)
        {
            auto uncompressed_data = read_compressed(file);
            std::string data(reinterpret_cast<char*>(&uncompressed_data[0]), uncompressed_data.size());
            std::istringstream data_stream(data, std::ios::binary);
            data_stream.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
            return read_vector<DataType>(data_stream, elements);
        }

        bool is_tr5(trview::Activity& activity, LevelVersion version, const std::wstring& filename)
        {
            if (version != LevelVersion::Tomb4)
            {
                return false;
            }

            activity.log("Checking file extension to determine whether this is a Tomb5 level");
            std::wstring transformed;
            std::transform(filename.begin(), filename.end(), std::back_inserter(transformed), towupper);
            return transformed.find(L".TRC") != filename.npos;
        }

        void skip(std::istream& file, uint32_t size)
        {
            file.seekg(size, std::ios::cur);
        }

        void skip_xela(std::istream& file)
        {
            skip(file, 4);
        }

        void load_tr1_4_room(trview::Activity& activity, std::istream& file, tr3_room& room, LevelVersion version)
        {
            activity.log("Reading room info");
            room.info = convert_room_info(read<tr1_4_room_info>(file));
            activity.log("Read room info");

            activity.log("Reading number of data words");
            uint32_t NumDataWords = read<uint32_t>(file);
            activity.log(std::format("{} data words to process", NumDataWords));

            // Read actual room data.
            if (NumDataWords > 0)
            {
                activity.log("Reading vertices");
                if (version == LevelVersion::Tomb1)
                {
                    room.data.vertices = convert_vertices(read_vector<int16_t, tr_room_vertex>(file));
                }
                else
                {
                    room.data.vertices = read_vector<int16_t, tr3_room_vertex>(file);
                }
                activity.log(std::format("Read {} vertices", room.data.vertices.size()));

                activity.log("Reading rectangles");
                room.data.rectangles = convert_rectangles(read_vector<int16_t, tr_face4>(file));
                activity.log(std::format("Read {} rectangles", room.data.rectangles.size()));
                activity.log("Reading triangles");
                room.data.triangles = convert_triangles(read_vector<int16_t, tr_face3>(file));
                activity.log(std::format("Read {} triangles", room.data.triangles.size()));
                activity.log("Reading sprites");
                room.data.sprites = read_vector<int16_t, tr_room_sprite>(file);
                activity.log(std::format("Read {} sprites", room.data.sprites.size()));
            }

            activity.log("Reading portals");
            room.portals = read_vector<uint16_t, tr_room_portal>(file);
            activity.log(std::format("Read {} portals", room.portals.size()));

            activity.log("Reading number of z sectors");
            room.num_z_sectors = read<uint16_t>(file);
            activity.log(std::format("There are {} z sectors", room.num_z_sectors));
            activity.log("Reading number of x sectors");
            room.num_x_sectors = read<uint16_t>(file);
            activity.log(std::format("There are {} x sectors", room.num_x_sectors));
            activity.log(std::format("Reading {} sectors", room.num_z_sectors * room.num_x_sectors));
            room.sector_list = read_vector<tr_room_sector>(file, room.num_z_sectors * room.num_x_sectors);
            activity.log(std::format("Read {} sectors", room.sector_list.size()));

            if (version == LevelVersion::Tomb4)
            {
                activity.log("Reading room colour");
                room.room_colour = read<uint32_t>(file);
                activity.log(std::format("Read room colour {:X}", room.room_colour));
            }
            else
            {
                activity.log("Reading ambient intensity 1");
                room.ambient_intensity_1 = read<int16_t>(file);
                activity.log(std::format("Read ambient intensity 1: {}", room.ambient_intensity_1));

                if (version > LevelVersion::Tomb1)
                {
                    activity.log("Reading ambient intensity 2");
                    room.ambient_intensity_2 = read<int16_t>(file);
                    activity.log(std::format("Read ambient intensity 2: {}", room.ambient_intensity_2));
                }
            }

            if (version == LevelVersion::Tomb2)
            {
                activity.log("Reading light mode");
                room.light_mode = read<int16_t>(file);
                activity.log(std::format("Read light mode: {}", room.light_mode));
            }

            activity.log("Reading lights");
            switch (version)
            {
            case LevelVersion::Tomb1:
            {
                room.lights = convert_lights(read_vector<uint16_t, tr_room_light>(file));
                break;
            }
            case LevelVersion::Tomb2:
            {
                room.lights = convert_lights(read_vector<uint16_t, tr2_room_light>(file));
                break;
            }
            case LevelVersion::Tomb3:
            {
                room.lights = convert_lights(read_vector<uint16_t, tr3_room_light>(file));
                break;
            }
            case LevelVersion::Tomb4:
            {
                room.lights = convert_lights(read_vector<uint16_t, tr4_room_light>(file));
                break;
            }
            }
            activity.log(std::format("Read {} lights", room.lights.size()));

            activity.log("Reading static meshes");
            if (version == LevelVersion::Tomb1)
            {
                room.static_meshes = convert_room_static_meshes(read_vector<uint16_t, tr_room_staticmesh>(file));
            }
            else
            {
                room.static_meshes = read_vector<uint16_t, tr3_room_staticmesh>(file);
            }
            activity.log(std::format("Read {} static meshes", room.static_meshes.size()));

            activity.log("Reading alternate room");
            room.alternate_room = read<int16_t>(file);
            activity.log(std::format("Read alternate room: {}", room.alternate_room));
            activity.log("Reading flags");
            room.flags = read<int16_t>(file);
            activity.log(std::format("Read flags: {:X}", room.flags));

            if (version >= LevelVersion::Tomb3)
            {
                activity.log("Reading water scheme");
                room.water_scheme = read<uint8_t>(file);
                activity.log(std::format("Read water scheme: {}", room.water_scheme));
                activity.log("Reading reverb info");
                room.reverb_info = read<uint8_t>(file);
                activity.log(std::format("Read reverb info: {}", room.reverb_info));
                activity.log("Reading alternate group");
                room.alternate_group = read<uint8_t>(file);
                activity.log(std::format("Read alternate group: {}", room.alternate_group));
            }
        }

        void load_tr5_room(trview::Activity& activity, std::istream& file, tr3_room& room)
        {
            activity.log("Reading room data information");
            skip_xela(file);
            uint32_t room_data_size = read<uint32_t>(file);
            const uint32_t room_start = static_cast<uint32_t>(file.tellg());
            const uint32_t room_end = room_start + room_data_size;
            activity.log(std::format("Reading room data information. Data Size: {}", room_data_size));

            activity.log("Reading room header");
            const auto header = read<tr5_room_header>(file);

            // Copy useful data from the header to the room.
            room.info = header.info;
            room.num_x_sectors = header.num_x_sectors;
            room.num_z_sectors = header.num_z_sectors;
            room.colour = header.colour;
            room.reverb_info = header.reverb_info;
            room.alternate_group = header.alternate_group;
            room.water_scheme = header.water_scheme;
            room.alternate_room = header.alternate_room;
            room.flags = header.flags;
            activity.log("Read room header");

            // The offsets start measuring from this position, after all the header information.
            const uint32_t data_start = static_cast<uint32_t>(file.tellg());

            activity.log(std::format("Reading {} lights", header.num_lights));
            room.lights = convert_lights(read_vector<tr5_room_light>(file, header.num_lights));

            activity.log(std::format("Reading {} fog bulbs", header.num_fog_bulbs));
            auto fog_bulbs = read_vector<tr5_fog_bulb>(file, header.num_fog_bulbs);

            activity.log("Converting lights to fog buibs");
            uint32_t fog_bulb = 0;
            for (auto& light : room.lights)
            {
                if (*reinterpret_cast<uint32_t*>(&light.tr5.position.x) == 0xCDCDCDCD)
                {
                    const auto fog = fog_bulbs[fog_bulb++];
                    light.tr5.light_type = LightType::FogBulb;
                    light.tr5_fog = fog;
                }
            }

            file.seekg(data_start + header.start_sd_offset, std::ios::beg);

            activity.log(std::format("Reading {} sectors ({} x {})", room.num_x_sectors * room.num_z_sectors, room.num_x_sectors, room.num_z_sectors));
            room.sector_list = read_vector<tr_room_sector>(file, room.num_z_sectors * room.num_x_sectors);
            activity.log("Reading room portals");
            room.portals = read_vector<uint16_t, tr_room_portal>(file);
            activity.log(std::format("Read {} room portals", room.portals.size()));

            // Separator
            skip(file, 2);

            file.seekg(data_start + header.end_portal_offset, std::ios::beg);
            activity.log(std::format("Reading {} static meshes", header.num_static_meshes));
            room.static_meshes = read_vector<tr3_room_staticmesh>(file, header.num_static_meshes);

            file.seekg(data_start + header.layer_offset, std::ios::beg);
            activity.log(std::format("Reading {} layers", header.num_layers));
            auto layers = read_vector<tr5_room_layer>(file, header.num_layers);

            file.seekg(data_start + header.poly_offset, std::ios::beg);
            uint16_t vertex_offset = 0;
            int32_t layer_number = 0;
            for (const auto& layer : layers)
            {
                activity.log(std::format("Reading {} rectangles for layer {}", layer.num_rectangles, layer_number));
                auto rects = read_vector<tr4_mesh_face4>(file, layer.num_rectangles);
                for (auto& rect : rects)
                {
                    for (auto& v : rect.vertices)
                    {
                        v += vertex_offset;
                    }
                }
                std::copy(rects.begin(), rects.end(), std::back_inserter(room.data.rectangles));

                activity.log(std::format("Reading {} triangles for layer {}", layer.num_triangles, layer_number));
                auto tris = read_vector<tr4_mesh_face3>(file, layer.num_triangles);
                for (auto& tri : tris)
                {
                    for (auto& v : tri.vertices)
                    {
                        v += vertex_offset;
                    }
                }
                std::copy(tris.begin(), tris.end(), std::back_inserter(room.data.triangles));

                vertex_offset += layer.num_vertices;
                ++layer_number;
            }

            file.seekg(data_start + header.vertices_offset, std::ios::beg);
            layer_number = 0;
            for (const auto& layer : layers)
            {
                activity.log(std::format("Reading {} vertices for layer {}", layer.num_vertices, layer_number));
                auto verts = convert_vertices(read_vector<tr5_room_vertex>(file, layer.num_vertices));
                std::copy(verts.begin(), verts.end(), std::back_inserter(room.data.vertices));
                ++layer_number;
            }

            file.seekg(room_end, std::ios::beg);
        }
    }

    namespace
    {
        void decrypt(std::ifstream& file)
        {
            const std::array<uint8_t, 1600> table
            {
                0x38, 0x22, 0xE9, 0x4F, 0x20, 0xF1, 0x37, 0x69, 0x15, 0x13, 0xD3, 0x90, 0x17, 0x84, 0xC2, 0x09, 0xAD, 0x04, 0xBE, 0xD2, 0x52, 0x18, 0x0B, 0xEC, 0x47, 0x38, 0xA8, 0x14, 0x8D, 0xAA, 0x97, 0xD0, 0xDF, 0x29, 0x92, 0x99, 0xC8, 0xF8, 0xDE, 0x70, 0xBD, 0x1A, 0x7C, 0xDB, 0xBF, 0x8B, 0x27, 0x53, 0x56, 0x4E, 0x67, 0x1D, 0xFA, 0x1E, 0xB3, 0xF2, 0xEF, 0x3F, 0x51, 0x5E, 0x35, 0xB0, 0xFB, 0xD6, 0x88, 0x2F, 0x3B, 0xA0, 0x2D, 0x43, 0x87, 0xBA, 0x22, 0x64,
                0x25, 0xE1, 0x68, 0xD5, 0xCF, 0x5A, 0xFE, 0x54, 0x0F, 0x23, 0xA2, 0x24, 0x5C, 0x3E, 0x98, 0x89, 0xF9, 0x65, 0xDD, 0xB7, 0xA3, 0xDD, 0x31, 0x7A, 0xE3, 0xA6, 0xD4, 0x4A, 0x30, 0xC0, 0xCA, 0x6A, 0xCD, 0xE8, 0x10, 0xDC, 0x78, 0xA4, 0xA7, 0x9F, 0xB7, 0x2A, 0x4B, 0x6F, 0x04, 0x44, 0x40, 0x8F, 0xA1, 0xAF, 0x86, 0x02, 0x4C, 0x28, 0xD9, 0x80, 0x8C, 0xF0, 0x7D, 0x94, 0xD8, 0xC7, 0x72, 0xB4, 0x32, 0x33, 0xB8, 0xE2, 0x21, 0xAB, 0x0C, 0xA5, 0x1C, 0x74,
                0xF3, 0x75, 0xAC, 0x8E, 0xE8, 0x96, 0x07, 0xB5, 0xEA, 0x08, 0xF4, 0x2E, 0x82, 0xCA, 0xF0, 0xF7, 0x26, 0x9A, 0x81, 0x12, 0x1B, 0xBB, 0xDA, 0x39, 0x61, 0x2D, 0xC9, 0xB1, 0xB4, 0xAC, 0xC4, 0x7B, 0x9C, 0xBF, 0x11, 0x95, 0x91, 0xE0, 0xAE, 0xBC, 0x93, 0x0F, 0x9D, 0x35, 0x2B, 0xD1, 0x99, 0xFD, 0xCE, 0xA1, 0xE5, 0x19, 0xC3, 0xC1, 0x83, 0x83, 0x0A, 0x34, 0x71, 0xFB, 0x5D, 0xF6, 0x6D, 0xC5, 0x01, 0xC6, 0xB9, 0x9B, 0xF5, 0xE7, 0x57, 0x07, 0x3C, 0x59,
                0x46, 0x7F, 0xD2, 0xD7, 0x41, 0x49, 0x77, 0xEB, 0x8E, 0x1F, 0x6C, 0x0D, 0x2C, 0x8A, 0xB2, 0x7E, 0x1A, 0x03, 0x05, 0xFC, 0x16, 0xCB, 0xA9, 0xCC, 0x62, 0xE5, 0x9E, 0xED, 0xFF, 0x0E, 0xE4, 0x5F, 0xEE, 0x85, 0x38, 0x22, 0xE9, 0x4F, 0x20, 0xF1, 0x37, 0x69, 0x15, 0x13, 0xD3, 0x90, 0x17, 0x84, 0xC2, 0x09, 0xAD, 0x04, 0xBE, 0xD2, 0x52, 0x18, 0x0B, 0xEC, 0x47, 0x38, 0xA8, 0x14, 0x8D, 0xAA, 0x97, 0xD0, 0xDF, 0x29, 0x92, 0x99, 0xC8, 0xF8, 0xDE, 0x70,
                0xBD, 0x1A, 0x7C, 0xDB, 0xBF, 0x8B, 0x27, 0x53, 0x56, 0x4E, 0x67, 0x1D, 0xFA, 0x1E, 0xB3, 0xF2, 0xEF, 0x3F, 0x51, 0x5E, 0x35, 0xB0, 0xFB, 0xD6, 0x88, 0x2F, 0x3B, 0xA0, 0x2D, 0x43, 0x87, 0xBA, 0x22, 0x64, 0x25, 0xE1, 0x68, 0xD5, 0xCF, 0x5A, 0xFE, 0x54, 0x0F, 0x23, 0xA2, 0x24, 0x5C, 0x3E, 0x98, 0x89, 0xF9, 0x65, 0xDD, 0xB7, 0xA3, 0xDD, 0x31, 0x7A, 0xE3, 0xA6, 0xD4, 0x4A, 0x30, 0xC0, 0xCA, 0x6A, 0xCD, 0xE8, 0x10, 0xDC, 0x78, 0xA4, 0xA7, 0x9F,
                0xB7, 0x2A, 0x4B, 0x6F, 0x04, 0x44, 0x40, 0x8F, 0xA1, 0xAF, 0x86, 0x02, 0x4C, 0x28, 0xD9, 0x80, 0x8C, 0xF0, 0x7D, 0x94, 0xD8, 0xC7, 0x72, 0xB4, 0x32, 0x33, 0xB8, 0xE2, 0x21, 0xAB, 0x0C, 0xA5, 0x1C, 0x74, 0xF3, 0x75, 0xAC, 0x8E, 0xE8, 0x96, 0x07, 0xB5, 0xEA, 0x08, 0xF4, 0x2E, 0x82, 0xCA, 0xF0, 0xF7, 0x26, 0x9A, 0x81, 0x12, 0x1B, 0xBB, 0xDA, 0x39, 0x61, 0x2D, 0xC9, 0xB1, 0xB4, 0xAC, 0xC4, 0x7B, 0x9C, 0xBF, 0x11, 0x95, 0x91, 0xE0, 0xAE, 0xBC,
                0x93, 0x0F, 0x9D, 0x35, 0x2B, 0xD1, 0x99, 0xFD, 0xCE, 0xA1, 0xE5, 0x19, 0xC3, 0xC1, 0x83, 0x83, 0x0A, 0x34, 0x71, 0xFB, 0x5D, 0xF6, 0x6D, 0xC5, 0x01, 0xC6, 0xB9, 0x9B, 0xF5, 0xE7, 0x57, 0x07, 0x3C, 0x59, 0x46, 0x7F, 0xD2, 0xD7, 0x41, 0x49, 0x77, 0xEB, 0x8E, 0x1F, 0x6C, 0x0D, 0x2C, 0x8A, 0xB2, 0x7E, 0x1A, 0x03, 0x05, 0xFC, 0x16, 0xCB, 0xA9, 0xCC, 0x62, 0xE5, 0x9E, 0xED, 0xFF, 0x0E, 0xE4, 0x5F, 0xEE, 0x85, 0x38, 0x22, 0xE9, 0x4F, 0x20, 0xF1,
                0x37, 0x69, 0x15, 0x13, 0xD3, 0x90, 0x17, 0x84, 0xC2, 0x09, 0xAD, 0x04, 0xBE, 0xD2, 0x52, 0x18, 0x0B, 0xEC, 0x47, 0x38, 0xA8, 0x14, 0x8D, 0xAA, 0x97, 0xD0, 0xDF, 0x29, 0x92, 0x99, 0xC8, 0xF8, 0xDE, 0x70, 0xBD, 0x1A, 0x7C, 0xDB, 0xBF, 0x8B, 0x27, 0x53, 0x56, 0x4E, 0x67, 0x1D, 0xFA, 0x1E, 0xB3, 0xF2, 0xEF, 0x3F, 0x51, 0x5E, 0x35, 0xB0, 0xFB, 0xD6, 0x88, 0x2F, 0x3B, 0xA0, 0x2D, 0x43, 0x87, 0xBA, 0x22, 0x64, 0x25, 0xE1, 0x68, 0xD5, 0xCF, 0x5A,
                0xFE, 0x54, 0x0F, 0x23, 0xA2, 0x24, 0x5C, 0x3E, 0x98, 0x89, 0xF9, 0x65, 0xDD, 0xB7, 0xA3, 0xDD, 0x31, 0x7A, 0xE3, 0xA6, 0xD4, 0x4A, 0x30, 0xC0, 0xCA, 0x6A, 0xCD, 0xE8, 0x10, 0xDC, 0x78, 0xA4, 0xA7, 0x9F, 0xB7, 0x2A, 0x4B, 0x6F, 0x04, 0x44, 0x40, 0x8F, 0xA1, 0xAF, 0x86, 0x02, 0x4C, 0x28, 0xD9, 0x80, 0x8C, 0xF0, 0x7D, 0x94, 0xD8, 0xC7, 0x72, 0xB4, 0x32, 0x33, 0xB8, 0xE2, 0x21, 0xAB, 0x0C, 0xA5, 0x1C, 0x74, 0xF3, 0x75, 0xAC, 0x8E, 0xE8, 0x96,
                0x07, 0xB5, 0xEA, 0x08, 0xF4, 0x2E, 0x82, 0xCA, 0xF0, 0xF7, 0x26, 0x9A, 0x81, 0x12, 0x1B, 0xBB, 0xDA, 0x39, 0x61, 0x2D, 0xC9, 0xB1, 0xB4, 0xAC, 0xC4, 0x7B, 0x9C, 0xBF, 0x11, 0x95, 0x91, 0xE0, 0xAE, 0xBC, 0x93, 0x0F, 0x9D, 0x35, 0x2B, 0xD1, 0x99, 0xFD, 0xCE, 0xA1, 0xE5, 0x19, 0xC3, 0xC1, 0x83, 0x83, 0x0A, 0x34, 0x71, 0xFB, 0x5D, 0xF6, 0x6D, 0xC5, 0x01, 0xC6, 0xB9, 0x9B, 0xF5, 0xE7, 0x57, 0x07, 0x3C, 0x59, 0x46, 0x7F, 0xD2, 0xD7, 0x41, 0x49,
                0x77, 0xEB, 0x8E, 0x1F, 0x6C, 0x0D, 0x2C, 0x8A, 0xB2, 0x7E, 0x1A, 0x03, 0x05, 0xFC, 0x16, 0xCB, 0xA9, 0xCC, 0x62, 0xE5, 0x9E, 0xED, 0xFF, 0x0E, 0xE4, 0x5F, 0xEE, 0x85, 0x38, 0x22, 0xE9, 0x4F, 0x20, 0xF1, 0x37, 0x69, 0x15, 0x13, 0xD3, 0x90, 0x17, 0x84, 0xC2, 0x09, 0xAD, 0x04, 0xBE, 0xD2, 0x52, 0x18, 0x0B, 0xEC, 0x47, 0x38, 0xA8, 0x14, 0x8D, 0xAA, 0x97, 0xD0, 0xDF, 0x29, 0x92, 0x99, 0xC8, 0xF8, 0xDE, 0x70, 0xBD, 0x1A, 0x7C, 0xDB, 0xBF, 0x8B,
                0x27, 0x53, 0x56, 0x4E, 0x67, 0x1D, 0xFA, 0x1E, 0xB3, 0xF2, 0xEF, 0x3F, 0x51, 0x5E, 0x35, 0xB0, 0xFB, 0xD6, 0x88, 0x2F, 0x3B, 0xA0, 0x2D, 0x43, 0x87, 0xBA, 0x22, 0x64, 0x25, 0xE1, 0x68, 0xD5, 0xCF, 0x5A, 0xFE, 0x54, 0x0F, 0x23, 0xA2, 0x24, 0x5C, 0x3E, 0x98, 0x89, 0xF9, 0x65, 0xDD, 0xB7, 0xA3, 0xDD, 0x31, 0x7A, 0xE3, 0xA6, 0xD4, 0x4A, 0x30, 0xC0, 0xCA, 0x6A, 0xCD, 0xE8, 0x10, 0xDC, 0x78, 0xA4, 0xA7, 0x9F, 0xB7, 0x2A, 0x4B, 0x6F, 0x04, 0xBC,
                0xB8, 0x08, 0x1A, 0x28, 0xFE, 0x79, 0xC4, 0x9F, 0x51, 0xF8, 0x04, 0x69, 0xF5, 0x0C, 0x50, 0x3F, 0xEA, 0x2D, 0xAA, 0xAA, 0x31, 0x5B, 0x98, 0x23, 0x84, 0x1E, 0x94, 0xEC, 0x6C, 0xED, 0x25, 0x07, 0x61, 0x0E, 0x7E, 0x2E, 0x63, 0x80, 0x6D, 0xA6, 0xF9, 0x43, 0x68, 0x6F, 0x9E, 0x13, 0xF8, 0x8A, 0x93, 0x33, 0x52, 0xB1, 0xD9, 0xA5, 0x41, 0x2A, 0x2C, 0x24, 0x3D, 0xF2, 0x15, 0x38, 0x89, 0x0D, 0x0A, 0x59, 0x27, 0x35, 0x0C, 0x86, 0x16, 0xAC, 0xA2, 0x49,
                0x11, 0x76, 0x47, 0x19, 0x5D, 0x90, 0x3C, 0x3A, 0xFA, 0xFB, 0x81, 0xAB, 0xE9, 0x74, 0xD4, 0x6E, 0xE5, 0x3D, 0x79, 0x3E, 0x32, 0x14, 0x6E, 0x5F, 0xCF, 0x7F, 0xB4, 0xD0, 0xBD, 0xF7, 0x4B, 0x50, 0xB9, 0xC0, 0xEE, 0x63, 0x06, 0x97, 0xE4, 0x84, 0xA3, 0x02, 0x2A, 0xF6, 0x92, 0x7A, 0x7D, 0x75, 0x8D, 0x44, 0x21, 0x45, 0xDA, 0x5E, 0x17, 0x66, 0x78, 0x85, 0x5C, 0xD7, 0x66, 0xFD, 0xAF, 0x9A, 0x62, 0xC7, 0x97, 0x6A, 0xAE, 0xE1, 0x8C, 0x8B, 0x4C, 0x09,
                0x8E, 0xFC, 0x3B, 0x81, 0x26, 0x7B, 0x36, 0x4A, 0xC9, 0x8F, 0x82, 0x65, 0xBE, 0xB0, 0x20, 0x8C, 0x05, 0x22, 0x0F, 0x48, 0x58, 0xA0, 0x0B, 0x12, 0x40, 0x70, 0x57, 0xE7, 0x35, 0x91, 0xF4, 0x53, 0x37, 0x04, 0x9F, 0xCB, 0xCE, 0xC6, 0xDE, 0x95, 0x72, 0x96, 0x2B, 0x6B, 0x67, 0xB6, 0xC8, 0xD6, 0xAD, 0x29, 0x73, 0x4F, 0x01, 0xA7, 0xB2, 0x18, 0xA4, 0xBB, 0xFF, 0x32, 0x99, 0xDB, 0x9D, 0x5A, 0xDF, 0x4E, 0x48, 0xD1, 0x77, 0xCC, 0x87, 0x9B, 0x1B, 0x9C,
                0xD3, 0xB5, 0x10, 0x01, 0x71, 0xDC, 0x56, 0x2F, 0x1C, 0x55, 0xA9, 0xF1, 0x5B, 0x1F, 0x4D, 0xC1, 0xA8, 0x39, 0x42, 0xE2, 0x46, 0x60, 0x88, 0x54, 0xEF, 0x1D, 0x1F, 0x17, 0x30, 0xA1, 0xC3, 0xE6, 0x7C, 0xBC, 0xB8, 0x08, 0x1A, 0x28, 0xFE, 0x79, 0xC4, 0x9F, 0x51, 0xF8, 0x04, 0x69, 0xF5, 0x0C, 0x50, 0x3F, 0xEA, 0x2D, 0xAA, 0xAA, 0x31, 0x5B, 0x98, 0x23, 0x84, 0x1E, 0x94, 0xEC, 0x6C, 0xED, 0x25, 0x07, 0x61, 0x0E, 0x7E, 0x2E, 0x63, 0x80, 0x6D, 0xA6,
                0xF9, 0x43, 0x68, 0x6F, 0x9E, 0x13, 0xF8, 0x8A, 0x93, 0x33, 0x52, 0xB1, 0xD9, 0xA5, 0x41, 0x2A, 0x2C, 0x24, 0x3D, 0xF2, 0x15, 0x38, 0x89, 0x0D, 0x0A, 0x59, 0x27, 0x35, 0x0C, 0x86, 0x16, 0xAC, 0xA2, 0x49, 0x11, 0x76, 0x47, 0x19, 0x5D, 0x90, 0x3C, 0x3A, 0xFA, 0xFB, 0x81, 0xAB, 0xE9, 0x74, 0xD4, 0x6E, 0xE5, 0x3D, 0x79, 0x3E, 0x32, 0x14, 0x6E, 0x5F, 0xCF, 0x7F, 0xB4, 0xD0, 0xBD, 0xF7, 0x4B, 0x50, 0xB9, 0xC0, 0xEE, 0x63, 0x06, 0x97, 0xE4, 0x84,
                0xA3, 0x02, 0x2A, 0xF6, 0x92, 0x7A, 0x7D, 0x75, 0x8D, 0x44, 0x21, 0x45, 0xDA, 0x5E, 0x17, 0x66, 0x78, 0x85, 0x5C, 0xD7, 0x66, 0xFD, 0xAF, 0x9A, 0x62, 0xC7, 0x97, 0x6A, 0xAE, 0xE1, 0x8C, 0x8B, 0x4C, 0x09, 0x8E, 0xFC, 0x3B, 0x81, 0x26, 0x7B, 0x36, 0x4A, 0xC9, 0x8F, 0x82, 0x65, 0xBE, 0xB0, 0x20, 0x8C, 0x05, 0x22, 0x0F, 0x48, 0x58, 0xA0, 0x0B, 0x12, 0x40, 0x70, 0x57, 0xE7, 0x35, 0x91, 0xF4, 0x53, 0x37, 0x04, 0x9F, 0xCB, 0xCE, 0xC6, 0xDE, 0x95,
                0x72, 0x96, 0x2B, 0x6B, 0x67, 0xB6, 0xC8, 0xD6, 0xAD, 0x29, 0x73, 0x4F, 0x01, 0xA7, 0xB2, 0x18, 0xA4, 0xBB, 0xFF, 0x32, 0x99, 0xDB, 0x9D, 0x5A, 0xDF, 0x4E, 0x48, 0xD1, 0x77, 0xCC, 0x87, 0x9B, 0x1B, 0x9C, 0xD3, 0xB5, 0x10, 0x01, 0x71, 0xDC, 0x56, 0x2F, 0x1C, 0x55, 0xA9, 0xF1, 0x5B, 0x1F, 0x4D, 0xC1, 0xA8, 0x39, 0x42, 0xE2, 0x46, 0x60, 0x88, 0x54, 0xEF, 0x1D, 0x1F, 0x17, 0x30, 0xA1, 0xC3, 0xE6, 0x7C, 0xBC, 0xB8, 0x08, 0x1A, 0x28, 0xFE, 0x79,
                0xC4, 0x9F, 0x51, 0xF8, 0x04, 0x69, 0xF5, 0x0C, 0x50, 0x3F, 0xEA, 0x2D, 0xAA, 0xAA, 0x31, 0x5B, 0x98, 0x23, 0x84, 0x1E, 0x94, 0xEC, 0x6C, 0xED, 0x25, 0x07, 0x61, 0x0E, 0x7E, 0x2E, 0x63, 0x80, 0x6D, 0xA6, 0xF9, 0x43, 0x68, 0x6F, 0x9E, 0x13, 0xF8, 0x8A, 0x93, 0x33, 0x52, 0xB1, 0xD9, 0xA5, 0x41, 0x2A, 0x2C, 0x24, 0x3D, 0xF2, 0x15, 0x38, 0x89, 0x0D, 0x0A, 0x59, 0x27, 0x35, 0x0C, 0x86, 0x16, 0xAC, 0xA2, 0x49, 0x11, 0x76, 0x47, 0x19, 0x5D, 0x90,
                0x3C, 0x3A, 0xFA, 0xFB, 0x81, 0xAB, 0xE9, 0x74, 0xD4, 0x6E, 0xE5, 0x3D, 0x79, 0x3E, 0x32, 0x14, 0x6E, 0x5F, 0xCF, 0x7F, 0xB4, 0xD0, 0xBD, 0xF7, 0x4B, 0x50, 0xB9, 0xC0, 0xEE, 0x63, 0x06, 0x97, 0xE4, 0x84, 0xA3, 0x02, 0x2A, 0xF6, 0x92, 0x7A, 0x7D, 0x75, 0x8D, 0x44, 0x21, 0x45, 0xDA, 0x5E, 0x17, 0x66, 0x78, 0x85, 0x5C, 0xD7, 0x66, 0xFD, 0xAF, 0x9A, 0x62, 0xC7, 0x97, 0x6A, 0xAE, 0xE1, 0x8C, 0x8B, 0x4C, 0x09, 0x8E, 0xFC, 0x3B, 0x81, 0x26, 0x7B,
                0x36, 0x4A, 0xC9, 0x8F, 0x82, 0x65, 0xBE, 0xB0, 0x20, 0x8C, 0x05, 0x22, 0x0F, 0x48, 0x58, 0xA0, 0x0B, 0x12, 0x40, 0x70, 0x57, 0xE7, 0x35, 0x91, 0xF4, 0x53, 0x37, 0x04, 0x9F, 0xCB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            };

            const std::array<uint8_t, 800> table2
            {
                0x00, 0x11, 0x07, 0x09, 0x06, 0x19, 0x01, 0x18, 0x30, 0x1A, 0x0B, 0x16, 0x08, 0x13, 0x15, 0x12, 0x1B, 0x2C, 0x17, 0x0A, 0x23, 0x2B, 0x21, 0x1D, 0x1E, 0x29, 0x03, 0x1C, 0x0C, 0x28, 0x26, 0x24, 0x27, 0x0D, 0x0E, 0x2E, 0x05, 0x2D, 0x22, 0x31, 0x1F, 0x2A, 0x02, 0x10, 0x04, 0x14, 0x0F, 0x2F, 0x20, 0x25, 0x12, 0x20, 0x0F, 0x00, 0x07, 0x0D, 0x0E, 0x10, 0x30, 0x2B, 0x08, 0x21, 0x28, 0x16, 0x17, 0x29, 0x06, 0x2C, 0x1E, 0x31, 0x0A, 0x1D, 0x26, 0x18,
                0x23, 0x1F, 0x2E, 0x05, 0x24, 0x03, 0x13, 0x22, 0x15, 0x1A, 0x0C, 0x01, 0x09, 0x25, 0x2D, 0x2A, 0x2F, 0x14, 0x27, 0x04, 0x0B, 0x11, 0x02, 0x19, 0x1C, 0x1B, 0x26, 0x18, 0x0C, 0x00, 0x27, 0x2F, 0x07, 0x10, 0x17, 0x08, 0x19, 0x1D, 0x0E, 0x1A, 0x0F, 0x2C, 0x2D, 0x25, 0x0B, 0x09, 0x15, 0x05, 0x1E, 0x2A, 0x21, 0x24, 0x14, 0x28, 0x29, 0x03, 0x06, 0x13, 0x01, 0x11, 0x16, 0x22, 0x23, 0x02, 0x1F, 0x30, 0x2E, 0x12, 0x31, 0x20, 0x0A, 0x2B, 0x04, 0x1B,
                0x1C, 0x0D, 0x2D, 0x0A, 0x26, 0x05, 0x17, 0x11, 0x13, 0x21, 0x04, 0x2A, 0x1D, 0x18, 0x06, 0x12, 0x31, 0x1E, 0x24, 0x00, 0x19, 0x0E, 0x07, 0x1C, 0x22, 0x28, 0x10, 0x08, 0x20, 0x1F, 0x0B, 0x1B, 0x14, 0x15, 0x23, 0x01, 0x09, 0x2E, 0x1A, 0x29, 0x2C, 0x0F, 0x16, 0x2B, 0x02, 0x2F, 0x0C, 0x30, 0x0D, 0x25, 0x27, 0x03, 0x1B, 0x04, 0x0A, 0x2B, 0x0C, 0x02, 0x16, 0x12, 0x17, 0x07, 0x1E, 0x11, 0x22, 0x01, 0x18, 0x1C, 0x2A, 0x0B, 0x21, 0x20, 0x23, 0x1F,
                0x0F, 0x2C, 0x14, 0x28, 0x24, 0x19, 0x09, 0x2E, 0x10, 0x1A, 0x00, 0x0D, 0x29, 0x08, 0x06, 0x03, 0x26, 0x25, 0x15, 0x31, 0x27, 0x13, 0x1D, 0x2D, 0x2F, 0x0E, 0x30, 0x05, 0x06, 0x17, 0x15, 0x28, 0x08, 0x12, 0x03, 0x1A, 0x27, 0x0E, 0x2B, 0x2F, 0x23, 0x16, 0x0A, 0x07, 0x20, 0x2D, 0x0F, 0x2C, 0x1F, 0x1D, 0x24, 0x10, 0x21, 0x00, 0x1C, 0x01, 0x29, 0x0B, 0x02, 0x18, 0x05, 0x26, 0x25, 0x04, 0x11, 0x19, 0x22, 0x2A, 0x2E, 0x13, 0x0C, 0x31, 0x1B, 0x30,
                0x09, 0x1E, 0x14, 0x0D, 0x06, 0x25, 0x10, 0x17, 0x0B, 0x12, 0x27, 0x15, 0x28, 0x00, 0x21, 0x1B, 0x2A, 0x04, 0x0D, 0x24, 0x14, 0x30, 0x03, 0x19, 0x07, 0x29, 0x0E, 0x08, 0x0A, 0x0F, 0x2D, 0x26, 0x2B, 0x2C, 0x22, 0x0C, 0x02, 0x1D, 0x2F, 0x18, 0x1C, 0x2E, 0x05, 0x01, 0x1E, 0x16, 0x13, 0x1A, 0x23, 0x09, 0x20, 0x11, 0x31, 0x1F, 0x2A, 0x1B, 0x0F, 0x07, 0x21, 0x0E, 0x14, 0x13, 0x0C, 0x25, 0x11, 0x00, 0x0B, 0x19, 0x1D, 0x0A, 0x15, 0x08, 0x20, 0x05,
                0x18, 0x1E, 0x04, 0x1C, 0x22, 0x12, 0x2D, 0x2B, 0x24, 0x09, 0x28, 0x29, 0x2C, 0x01, 0x17, 0x0D, 0x2F, 0x27, 0x03, 0x26, 0x16, 0x30, 0x1A, 0x23, 0x2E, 0x10, 0x31, 0x06, 0x1F, 0x02, 0x1B, 0x0E, 0x0C, 0x09, 0x28, 0x01, 0x02, 0x04, 0x29, 0x17, 0x13, 0x21, 0x08, 0x16, 0x15, 0x18, 0x0A, 0x2F, 0x19, 0x1E, 0x24, 0x1D, 0x22, 0x2B, 0x06, 0x26, 0x05, 0x20, 0x23, 0x0B, 0x14, 0x12, 0x30, 0x2A, 0x07, 0x31, 0x1F, 0x0F, 0x10, 0x1A, 0x2D, 0x03, 0x25, 0x2C,
                0x00, 0x0D, 0x1C, 0x2E, 0x27, 0x11, 0x1D, 0x00, 0x0B, 0x04, 0x10, 0x12, 0x0D, 0x14, 0x19, 0x20, 0x2C, 0x01, 0x1E, 0x25, 0x07, 0x06, 0x09, 0x1A, 0x16, 0x03, 0x2A, 0x21, 0x23, 0x24, 0x1F, 0x0F, 0x02, 0x0E, 0x22, 0x13, 0x27, 0x2E, 0x2D, 0x29, 0x08, 0x15, 0x31, 0x0A, 0x11, 0x26, 0x2F, 0x1C, 0x28, 0x2B, 0x1B, 0x05, 0x17, 0x30, 0x18, 0x0C, 0x07, 0x18, 0x0A, 0x14, 0x06, 0x1C, 0x09, 0x10, 0x29, 0x26, 0x2B, 0x30, 0x2F, 0x23, 0x16, 0x08, 0x13, 0x20,
                0x03, 0x1B, 0x2C, 0x1E, 0x24, 0x21, 0x22, 0x2E, 0x1D, 0x11, 0x27, 0x02, 0x05, 0x19, 0x04, 0x00, 0x25, 0x28, 0x12, 0x1A, 0x0E, 0x2A, 0x0F, 0x0B, 0x01, 0x31, 0x2D, 0x0D, 0x1F, 0x15, 0x0C, 0x17, 0x06, 0x25, 0x10, 0x17, 0x0B, 0x12, 0x27, 0x15, 0x28, 0x00, 0x21, 0x1B, 0x2A, 0x04, 0x0D, 0x24, 0x14, 0x30, 0x03, 0x19, 0x07, 0x29, 0x0E, 0x08, 0x0A, 0x0F, 0x2D, 0x26, 0x2B, 0x2C, 0x22, 0x0C, 0x02, 0x1D, 0x2F, 0x18, 0x1C, 0x2E, 0x05, 0x01, 0x1E, 0x16,
                0x13, 0x1A, 0x23, 0x09, 0x20, 0x11, 0x31, 0x1F, 0x2A, 0x1B, 0x0F, 0x07, 0x21, 0x0E, 0x14, 0x13, 0x0C, 0x25, 0x11, 0x00, 0x0B, 0x19, 0x1D, 0x0A, 0x15, 0x08, 0x20, 0x05, 0x18, 0x1E, 0x04, 0x1C, 0x22, 0x12, 0x2D, 0x2B, 0x24, 0x09, 0x28, 0x29, 0x2C, 0x01, 0x17, 0x0D, 0x2F, 0x27, 0x03, 0x26, 0x16, 0x30, 0x1A, 0x23, 0x2E, 0x10, 0x31, 0x06, 0x1F, 0x02, 0x1B, 0x0E, 0x0C, 0x09, 0x28, 0x01, 0x02, 0x04, 0x29, 0x17, 0x13, 0x21, 0x08, 0x16, 0x15, 0x18,
                0x0A, 0x2F, 0x19, 0x1E, 0x24, 0x1D, 0x22, 0x2B, 0x06, 0x26, 0x05, 0x20, 0x23, 0x0B, 0x14, 0x12, 0x30, 0x2A, 0x07, 0x31, 0x1F, 0x0F, 0x10, 0x1A, 0x2D, 0x03, 0x25, 0x2C, 0x00, 0x0D, 0x1C, 0x2E, 0x27, 0x11, 0x1D, 0x00, 0x0B, 0x04, 0x10, 0x12, 0x0D, 0x14, 0x19, 0x20, 0x2C, 0x01, 0x1E, 0x25, 0x07, 0x06, 0x09, 0x1A, 0x16, 0x03, 0x2A, 0x21, 0x23, 0x24, 0x1F, 0x0F, 0x02, 0x0E, 0x22, 0x13, 0x27, 0x2E, 0x2D, 0x29, 0x08, 0x15, 0x31, 0x0A, 0x11, 0x26,
                0x2F, 0x1C, 0x28, 0x2B, 0x1B, 0x05, 0x17, 0x30, 0x18, 0x0C, 0x07, 0x18, 0x0A, 0x14, 0x06, 0x1C, 0x09, 0x10, 0x29, 0x26, 0x2B, 0x30, 0x2F, 0x23, 0x16, 0x08, 0x13, 0x20, 0x03, 0x1B, 0x2C, 0x1E, 0x24, 0x21, 0x22, 0x2E, 0x1D, 0x11, 0x27, 0x02, 0x05, 0x19, 0x04, 0x00, 0x25, 0x28, 0x12, 0x1A, 0x0E, 0x2A, 0x0F, 0x0B, 0x01, 0x31, 0x2D, 0x0D, 0x1F, 0x15, 0x0C, 0x17

            };

            file.seekg(0, std::ios::end);
            const auto length = file.tellg();
            std::vector<uint8_t> bytes(length, 0);
            file.seekg(0, std::ios::beg);
            file.read(reinterpret_cast<char*>(&bytes[0]), length);

            // Remove the 'c' from TR4c
            bytes[3] = 'c';

            uint32_t* data = reinterpret_cast<uint32_t*>(&bytes[0xe]);
            int x = 0;

            uint8_t buffer[50];

            do
            {
                uint32_t first = *data;
                uint32_t* p2 = data + 1;
                int x2 = x + 1;

                int index = (first >> ((uint8_t)x2 & 0x1f) & 0xf) * 99;
                if ((((char)data + 4 | (char)index + 0x80u) & 3) == 0)
                {
                    for (int z = 0; z < 12; ++z)
                    {
                        reinterpret_cast<uint32_t*>(buffer)[z]
                            = data[z + 1] ^ *(uint32_t*)(&table[index + 4 * z]);
                    }
                    buffer[48] = table[index + 48] ^ *(uint8_t*)(data + 0xd);
                    buffer[49] = table[index + 49] ^ *(uint8_t*)((int)data + 0x35);
                }
                else
                {
                    for (int y = 0; y < 0x32; ++y)
                    {
                        *(uint8_t*)((int)buffer + y) = *(uint8_t*)((int)data + y + 4) ^ table[y + index];
                    }
                }

                int i = 0;
                for (i = 0; i < 0x32; ++i)
                {
                    *(uint8_t*)((int)data + i + 4)
                        = *(uint8_t*)((int)buffer + (uint32_t)(uint8_t)table2[i + (first >> ((char)x + 5U & 0x1f) & 0xf) * 0x32]);
                }
                
                x = i;
                if ((((char)data + 0x36U | (char)index + 0xb2U) & 3) != 0)
                {
                    do {
                        uint8_t* pbVar1 = (uint8_t*)((int)data + x + 4);
                        *pbVar1 = *pbVar1 ^ table[x + index];
                        x = x + 1;
                    } while (x != 99);
                }
                else
                {
                    data[13] ^= *reinterpret_cast<const uint32_t*>(&table[index + 50]);
                    data[14] ^= *reinterpret_cast<const uint32_t*>(&table[index + 54]);
                    data[15] ^= *reinterpret_cast<const uint32_t*>(&table[index + 58]);
                    data[16] ^= *reinterpret_cast<const uint32_t*>(&table[index + 62]);
                    data[17] ^= *reinterpret_cast<const uint32_t*>(&table[index + 66]);
                    data[18] ^= *reinterpret_cast<const uint32_t*>(&table[index + 70]);
                    data[19] ^= *reinterpret_cast<const uint32_t*>(&table[index + 74]);
                    data[20] ^= *reinterpret_cast<const uint32_t*>(&table[index + 78]);
                    data[21] ^= *reinterpret_cast<const uint32_t*>(&table[index + 82]);
                    data[22] ^= *reinterpret_cast<const uint32_t*>(&table[index + 86]);
                    data[23] ^= *reinterpret_cast<const uint32_t*>(&table[index + 90]);
                    data[24] ^= *reinterpret_cast<const uint32_t*>(&table[index + 94]);
                    data[25] ^= *reinterpret_cast<const uint32_t*>(&table[index + 98]);
                }

                if (data = (uint32_t*)((int)p2 + first + 4), x = x2, x2 == 4)
                {
                    std::ofstream out("C:\\Users\\Chris\\AppData\\Local\\Temp\\Temper\\feoasadi\\converted.tr4", std::ios::out | std::ios::binary);
                    out.write(reinterpret_cast<char*>(&bytes[0]), bytes.size());
                    break;
                }
                
            } while (true);
        }
    }

    Level::Level(const std::string& filename, const std::shared_ptr<trview::ILog>& log)
        : _log(log)
    {
        // Load the level from the file.
        auto last_index = std::min(filename.find_last_of('\\'), filename.find_last_of('/'));
        _name = last_index == filename.npos ? filename : filename.substr(std::min(last_index + 1, filename.size()));

        trview::Activity activity(log, "IO", "Load Level " + _name);

        try
        {
            // Convert the filename to UTF-16
            auto converted = trview::to_utf16(filename);
            activity.log(std::format("Opening file \"{}\"", filename));

            std::ifstream file;
            file.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
            file.open(converted.c_str(), std::ios::binary);

            activity.log(std::format("Opened file \"{}\"", filename));

            activity.log("Reading version number from file");
            uint32_t raw_version = read<uint32_t>(file);
            _version = convert_level_version(raw_version);
            activity.log(std::format("Version number is {:X} ({})", raw_version, to_string(_version)));

            decrypt(file);

            if (raw_version == 0x63345254)
            {
                decrypt(file);
                throw LevelEncryptedException();
            }

            if (is_tr5(activity, _version, converted))
            {
                _version = LevelVersion::Tomb5;
                activity.log(std::format("Version number is {:X} ({})", raw_version, to_string(_version)));
            }

            if (_version >= LevelVersion::Tomb4)
            {
                load_tr4(activity, file);
                return;
            }

            if (_version > LevelVersion::Tomb1)
            {
                activity.log("Reading 8-bit palette");
                _palette = read_vector<tr_colour>(file, 256);
                activity.log("Reading 16-bit palette");
                _palette16 = read_vector<tr_colour4>(file, 256);
            }

            activity.log("Reading textiles");
            _num_textiles = read<uint32_t>(file);
            activity.log(std::format("Reading {} 8-bit textiles", _num_textiles));
            _textile8 = read_vector<tr_textile8>(file, _num_textiles);

            if (_version > LevelVersion::Tomb1)
            {
                activity.log(std::format("Reading {} 16-bit textiles", _num_textiles));
                _textile16 = read_vector<tr_textile16>(file, _num_textiles);
            }

            load_level_data(activity, file);
            generate_meshes(_mesh_data);
        }
        catch (const LevelEncryptedException&)
        {
            activity.log(trview::Message::Status::Error, "Level is encrypted, aborting");
            throw;
        }
        catch (const std::exception& e)
        {
            activity.log(trview::Message::Status::Error, std::format("Level failed to load: {}", e.what()));
            throw LevelLoadException();
        }
    }

    Level::~Level()
    {
    }

    void Level::generate_meshes(const std::vector<uint16_t>& mesh_data)
    {
        // As well as reading the actual mesh data, generate a map of mesh_pointer to 
        // mesh. It seems that a lot of the pointers point to the same mesh.

        std::string data(reinterpret_cast<const char*>(&mesh_data[0]), mesh_data.size() * sizeof(uint16_t));
        std::istringstream stream(data, std::ios::binary);
        stream.exceptions(std::istream::failbit | std::istream::badbit | std::istream::eofbit);
        for (auto pointer : _mesh_pointers)
        {
            // Does the map already contain this mesh? If so, don't bother reading it again.
            auto found = _meshes.find(pointer);
            if (found != _meshes.end())
            {
                continue;
            }

            stream.seekg(pointer, std::ios::beg);

            tr_mesh mesh;
            mesh.centre = read<tr_vertex>(stream);
            mesh.coll_radius = read<int32_t>(stream);
            mesh.vertices = read_vector<int16_t, tr_vertex>(stream);

            int16_t normals = read<int16_t>(stream);
            if (normals > 0)
            {
                mesh.normals = read_vector<tr_vertex>(stream, normals);
            }
            else
            {
                mesh.lights = read_vector<int16_t>(stream, abs(normals));
            }

            if (_version < LevelVersion::Tomb4)
            {
                mesh.textured_rectangles = convert_rectangles(read_vector<int16_t, tr_face4>(stream));
                mesh.textured_triangles = convert_triangles(read_vector<int16_t, tr_face3>(stream));
                mesh.coloured_rectangles = read_vector<int16_t, tr_face4>(stream);
                mesh.coloured_triangles = read_vector<int16_t, tr_face3>(stream);
            }
            else
            {
                mesh.textured_rectangles = read_vector<int16_t, tr4_mesh_face4>(stream);
                mesh.textured_triangles = read_vector<int16_t, tr4_mesh_face3>(stream);
            }

            _meshes.insert({ pointer, mesh });
        }
    }

    tr_colour Level::get_palette_entry8(uint32_t index) const
    {
        return _palette[index];
    }

    tr_colour4 Level::get_palette_entry_16(uint32_t index) const
    {
        return _palette16[index];
    }

    tr_colour4 Level::get_palette_entry(uint32_t index) const
    {
        if (index < _palette16.size())
        {
            return get_palette_entry_16(index);
        }
        return convert_to_colour4(get_palette_entry8(index));
    }

    tr_colour4 Level::get_palette_entry(uint32_t index8, uint32_t index16) const
    {
        if (index16 < _palette16.size())
        {
            return get_palette_entry_16(index16);
        }
        return convert_to_colour4(get_palette_entry8(index8));
    }

    uint32_t Level::num_textiles() const
    {
        return _num_textiles;
    }

    tr_textile8 Level::get_textile8(uint32_t index) const
    {
        return _textile8[index];
    }

    tr_textile16 Level::get_textile16(uint32_t index) const
    {
        return _textile16[index];
    }

    std::vector<uint32_t> Level::get_textile(uint32_t index) const
    {
        std::vector<uint32_t> results;
        results.reserve(256 * 256);

        if (index < _textile32.size())
        {
            const auto& textile = _textile32[index];
            std::transform(textile.Tile,
                textile.Tile + sizeof(textile.Tile) / sizeof(uint32_t),
                std::back_inserter(results),
                convert_textile32);
        }
        else if (index < _textile16.size())
        {
            const auto& textile = _textile16[index];
            std::transform(textile.Tile, 
                textile.Tile + sizeof(textile.Tile) / sizeof(uint16_t),  
                std::back_inserter(results),
                convert_textile16);
        }
        else
        {
            const auto& textile = _textile8[index];
            std::transform(textile.Tile,
                textile.Tile + sizeof(textile.Tile) / sizeof(uint8_t),
                std::back_inserter(results),
                [&](uint8_t entry_index)
            {
                // The first entry in the 8 bit palette is the transparent colour, so just return 
                // fully transparent instead of replacing it later.
                if (entry_index == 0)
                {
                    return 0x00000000u;
                }
                auto entry = get_palette_entry(entry_index);
                uint32_t value = 0xff000000 | entry.Blue << 16 | entry.Green << 8 | entry.Red;
                return value;
            });
        }

        return results;
    }

    uint32_t Level::num_rooms() const
    {
        return static_cast<uint32_t>(_rooms.size());
    }

    tr3_room Level::get_room(uint32_t index) const
    {
        return _rooms[index];
    }

    uint32_t Level::num_object_textures() const
    {
        return static_cast<uint32_t>(_object_textures.size());
    }

    tr_object_texture Level::get_object_texture(uint32_t index) const
    {
        return _object_textures[index];
    }

    uint32_t Level::num_floor_data() const
    {
        return static_cast<uint32_t>(_floor_data.size());
    }

    uint16_t Level::get_floor_data(uint32_t index) const
    {
        return _floor_data[index];
    }

    std::vector<std::uint16_t> 
    Level::get_floor_data_all() const
    {
        return _floor_data;
    }

    uint32_t Level::num_ai_objects() const
    {
        return static_cast<uint32_t>(_ai_objects.size());
    }

    tr4_ai_object Level::get_ai_object(uint32_t index) const
    {
        return _ai_objects[index];
    }

    uint32_t Level::num_entities() const
    {
        return static_cast<uint32_t>(_entities.size());
    }

    tr2_entity Level::get_entity(uint32_t index) const 
    {
        return _entities[index];
    }

    uint32_t Level::num_models() const
    {
        return static_cast<uint32_t>(_models.size());
    }

    tr_model Level::get_model(uint32_t index) const
    {
        return _models[index];
    }

    bool Level::get_model_by_id(uint32_t id, tr_model& output) const 
    {
        for (const auto& model : _models)
        {
            if (model.ID == id)
            {
                output = model;
                return true;
            }
        }
        return false;
    }

    uint32_t Level::num_static_meshes() const
    {
        return static_cast<uint32_t>(_static_meshes.size());
    }

    std::optional<tr_staticmesh> Level::get_static_mesh(uint32_t mesh_id) const
    {
        auto mesh = _static_meshes.find(mesh_id);
        if (mesh == _static_meshes.end())
        {
            return std::nullopt;
        }
        return mesh->second;
    }

    uint32_t Level::num_mesh_pointers() const
    {
        return static_cast<uint32_t>(_mesh_pointers.size());
    }

    tr_mesh Level::get_mesh_by_pointer(uint32_t mesh_pointer) const
    {
        auto index = _mesh_pointers[mesh_pointer];
        return _meshes.find(index)->second;
    }

    std::vector<tr_meshtree_node> Level::get_meshtree(uint32_t starting_index, uint32_t node_count) const
    {
        uint32_t index = starting_index;
        std::vector<tr_meshtree_node> nodes;
        for (uint32_t i = 0; i < node_count; ++i)
        {
            tr_meshtree_node node;
            node.Flags = _meshtree[index++];
            node.Offset_X = static_cast<int32_t>(_meshtree[index++]);
            node.Offset_Y = static_cast<int32_t>(_meshtree[index++]);
            node.Offset_Z = static_cast<int32_t>(_meshtree[index++]);
            nodes.push_back(node);
        }
        return nodes;
    }

    tr2_frame Level::get_frame(uint32_t frame_offset, uint32_t mesh_count) const
    {
        uint32_t offset = frame_offset;
        tr2_frame frame;
        frame.bb1x = _frames[offset++];
        frame.bb1y = _frames[offset++];
        frame.bb1z = _frames[offset++];
        frame.bb2x = _frames[offset++];
        frame.bb2y = _frames[offset++];
        frame.bb2z = _frames[offset++];
        frame.offsetx = _frames[offset++];
        frame.offsety = _frames[offset++];
        frame.offsetz = _frames[offset++];

        // Tomb Raider I has the mesh count in the frame structure - all other tombs
        // already know based on the number of meshes.
        if (_version == LevelVersion::Tomb1)
        {
            mesh_count = _frames[offset++];
        }

        for (uint32_t i = 0; i < mesh_count; ++i)
        {
            tr2_frame_rotation rotation;

            uint16_t next = 0;
            uint16_t data = 0;
            uint16_t mode = 0;

            // Tomb Raider I has reversed words and always uses the two word format.
            if (_version == LevelVersion::Tomb1)
            {
                next = _frames[offset++];
                data = _frames[offset++];
            }
            else
            {
                data = _frames[offset++];
                mode = data & 0xC000;
                if (!mode)
                {
                    next = _frames[offset++];
                }
            }

            if (mode)
            {
                float angle = 0;
                if (_version >= LevelVersion::Tomb4)
                {
                    angle = (data & 0x0fff) * PiMul2 / 4096.0f;
                }
                else
                {
                    angle = (data & 0x03ff) * PiMul2 / 1024.0f;
                }

                if (mode == 0x4000)
                {
                    rotation.x = angle;
                }
                else if (mode == 0x8000)
                {
                    rotation.y = angle;
                }
                else if (mode == 0xC000)
                {
                    rotation.z = angle;
                }
            }
            else
            {
                rotation.x = ((data & 0x3ff0) >> 4) * PiMul2 / 1024.0f;
                rotation.y = ((((data & 0x000f) << 6)) | ((next & 0xfc00) >> 10)) * PiMul2 / 1024.0f;
                rotation.z = (next & 0x03ff) * PiMul2 / 1024.0f;
            }
            frame.values.push_back(rotation);
        }
        return frame;
    }

    LevelVersion Level::get_version() const 
    {
        return _version;
    }

    bool Level::get_sprite_sequence_by_id(int32_t sprite_sequence_id, tr_sprite_sequence& output) const
    {
        auto found_sequence = std::find_if(_sprite_sequences.begin(), _sprite_sequences.end(), [=](const auto& sequence)
        {
            return sequence.SpriteID == sprite_sequence_id; 
        });

        if (found_sequence == _sprite_sequences.end())
        {
            return false;
        }

        output = *found_sequence;
        return true;
    }

    tr_sprite_texture Level::get_sprite_texture(uint32_t index) const
    {
        return _sprite_textures[index];
    }

    void Level::load_tr4(trview::Activity& activity, std::ifstream& file)
    {
        activity.log("Reading textile counts");
        uint16_t num_room_textiles = read<uint16_t>(file);
        uint16_t num_obj_textiles = read<uint16_t>(file);
        uint16_t num_bump_textiles = read<uint16_t>(file);
        activity.log(std::format("Textile counts - Room:{}, Object:{}, Bump:{}", num_room_textiles, num_obj_textiles, num_bump_textiles));
        _num_textiles = num_room_textiles + num_obj_textiles + num_bump_textiles;

        activity.log(std::format("Reading {} 32-bit textiles", _num_textiles));
        _textile32 = read_vector_compressed<tr_textile32>(file, _num_textiles);
        activity.log(std::format("Reading {} 16-bit textiles", _num_textiles));
        _textile16 = read_vector_compressed<tr_textile16>(file, _num_textiles);
        activity.log("Reading misc textiles");
        auto textile32_misc = read_vector_compressed<tr_textile32>(file, 2);

        if (_version == LevelVersion::Tomb5)
        {
            activity.log("Reading Lara type");
            _lara_type = read<uint16_t>(file);
            activity.log(std::format("Lara type: {}", _lara_type));
            activity.log("Reading weather type");
            _weather_type = read<uint16_t>(file);
            activity.log(std::format("Weather type: {}", _weather_type));
            activity.log("Skipping 28 unknown/padding bytes");
            file.seekg(28, std::ios::cur);
        }

        if (_version == LevelVersion::Tomb4)
        {
            activity.log("Reading and decompressing level data");
            std::vector<uint8_t> level_data = read_compressed(file);
            std::string data(reinterpret_cast<char*>(&level_data[0]), level_data.size());
            std::istringstream data_stream(data, std::ios::binary);
            load_level_data(activity, data_stream);
        }
        else
        {
            activity.log("Skipping uncompresed and compressed sizes - unused in Tomb5");
            skip(file, 8);
            load_level_data(activity, file);
        }

        if (_version == LevelVersion::Tomb5)
        {
            skip(file, 6);
        }

        activity.log("Reading sound sample count");
        uint32_t num_sound_samples = read<uint32_t>(file);
        std::vector<tr4_sample> sound_samples(num_sound_samples);
        activity.log(std::format("Reading {} sound samples", num_sound_samples));
        for (uint32_t i = 0; i < num_sound_samples; ++i)
        {
            sound_samples[i].sound_data = read_compressed(file);
        }

        activity.log("Generating meshes");
        generate_meshes(_mesh_data);
    }

    void Level::load_level_data(trview::Activity& activity, std::istream& file)
    {
        // Read unused value.
        read<uint32_t>(file);

        activity.log("Reading number of rooms");
        uint32_t num_rooms = 0;
        if (_version == LevelVersion::Tomb5)
        {
            num_rooms = read<uint32_t>(file);
        }
        else
        {
            num_rooms = read<uint16_t>(file);
        }

        activity.log(std::format("Reading {} rooms", num_rooms));
        for (auto i = 0u; i < num_rooms; ++i)
        {
            trview::Activity room_activity(activity, std::format("Room {}", i));
            room_activity.log(std::format("Reading room {}", i));
            tr3_room room;
            if (_version == LevelVersion::Tomb5)
            {
                load_tr5_room(room_activity, file, room);
            }
            else
            {
                load_tr1_4_room(room_activity, file, room, _version);
            }
            room_activity.log(std::format("Read room {}", i));
            _rooms.push_back(room);
        }

        activity.log("Reading floor data");
        _floor_data = read_vector<uint32_t, uint16_t>(file);
        activity.log(std::format("Read {} floor data", _floor_data.size()));

        activity.log("Reading mesh data");
        _mesh_data = read_vector<uint32_t, uint16_t>(file);
        activity.log(std::format("Read {} mesh data", _mesh_data.size()));

        activity.log("Reading mesh pointers");
        _mesh_pointers = read_vector<uint32_t, uint32_t>(file);
        activity.log(std::format("Read {} mesh pointers", _mesh_pointers.size()));

        activity.log("Reading animations");
        if (_version >= LevelVersion::Tomb4)
        {
            auto animations = read_vector<uint32_t, tr4_animation>(file);
        }
        else
        {
            std::vector<tr_animation> animations = read_vector<uint32_t, tr_animation>(file);
        }
        
        activity.log("Reading state changes");
        std::vector<tr_state_change> state_changes = read_vector<uint32_t, tr_state_change>(file);
        activity.log(std::format("Read {} state changes", state_changes.size()));

        activity.log("Reading anim dispatches");
        std::vector<tr_anim_dispatch> anim_dispatches = read_vector<uint32_t, tr_anim_dispatch>(file);
        activity.log(std::format("Read {} anim dispatches", anim_dispatches.size()));

        activity.log("Reading anim commands");
        std::vector<tr_anim_command> anim_commands = read_vector<uint32_t, tr_anim_command>(file);
        activity.log(std::format("Read {} anim commands", anim_commands.size()));

        activity.log("Reading mesh trees");
        _meshtree = read_vector<uint32_t, uint32_t>(file);
        activity.log(std::format("Read {} mesh trees", _meshtree.size()));

        activity.log("Reading frames");
        _frames = read_vector<uint32_t, uint16_t>(file);
        activity.log(std::format("Read {} frames", _frames.size()));

        activity.log("Reading models");
        if (_version < LevelVersion::Tomb5)
        {
            _models = read_vector<uint32_t, tr_model>(file);
        }
        else
        {
            _models = convert_models(read_vector<uint32_t, tr5_model>(file));
        }
        activity.log(std::format("Read {} models", _models.size()));

        activity.log("Reading static meshes");
        auto static_meshes = read_vector<uint32_t, tr_staticmesh>(file);
        activity.log(std::format("Read {} static meshes", static_meshes.size()));
        for (const auto& mesh : static_meshes)
        {
            _static_meshes.insert({ mesh.ID, mesh });
        }

        if (get_version() < LevelVersion::Tomb3)
        {
            activity.log("Reading object textures");
            _object_textures = read_vector<uint32_t, tr_object_texture>(file);
            activity.log(std::format("Read {} object textures", _object_textures.size()));
        }

        if (_version >= LevelVersion::Tomb4)
        {
            activity.log("Skipping SPR marker");
            // Skip past the 'SPR' marker.
            file.seekg(3, std::ios::cur);
            if (_version == LevelVersion::Tomb5)
            {
                activity.log("Skipping SPR null terminator");
                skip(file, 1);
            }
        }

        activity.log("Reading sprite textures");
        _sprite_textures = read_vector<uint32_t, tr_sprite_texture>(file);
        activity.log(std::format("Read {} sprite textures", _sprite_textures.size()));
        activity.log("Reading sprite sequences");
        _sprite_sequences = read_vector<uint32_t, tr_sprite_sequence>(file);
        activity.log(std::format("Read {} sprite sequences", _sprite_sequences.size()));

        // If this is Unfinished Business, the palette is here.
        // Need to do something about that, instead of just crashing.

        activity.log("Reading cameras");
        std::vector<tr_camera> cameras = read_vector<uint32_t, tr_camera>(file);
        activity.log(std::format("Read {} cameras", cameras.size()));

        if (_version >= LevelVersion::Tomb4)
        {
            activity.log("Reading flyby cameras");
            std::vector<tr4_flyby_camera> flyby_cameras = read_vector<uint32_t, tr4_flyby_camera>(file);
            activity.log(std::format("Read {} flyby cameras", flyby_cameras.size()));
        }

        activity.log("Reading sound sources");
        std::vector<tr_sound_source> sound_sources = read_vector<uint32_t, tr_sound_source>(file);
        activity.log(std::format("Read {} sound sources", sound_sources.size()));

        uint32_t num_boxes = 0;
        activity.log("Reading boxes");
        if (_version == LevelVersion::Tomb1)
        {
            std::vector<tr_box> boxes = read_vector<uint32_t, tr_box>(file);
            num_boxes = static_cast<uint32_t>(boxes.size());
        }
        else
        {
            std::vector<tr2_box> boxes = read_vector<uint32_t, tr2_box>(file);
            num_boxes = static_cast<uint32_t>(boxes.size());
        }
        activity.log(std::format("Read {} boxes", num_boxes));

        activity.log("Reading overlaps");
        std::vector<uint16_t> overlaps = read_vector<uint32_t, uint16_t>(file);
        activity.log(std::format("Read {} overlaps", overlaps.size()));

        activity.log("Reading zones");
        if (_version == LevelVersion::Tomb1)
        {
            std::vector<int16_t> zones = read_vector<int16_t>(file, num_boxes * 6);
            activity.log(std::format("Read {} zones", zones.size()));
        }
        else
        {
            std::vector<int16_t> zones = read_vector<int16_t>(file, num_boxes * 10);
            activity.log(std::format("Read {} zones", zones.size()));
        }

        activity.log("Reading animated textures");
        std::vector<uint16_t> animated_textures = read_vector<uint32_t, uint16_t>(file);
        activity.log(std::format("Read {} animated textures", animated_textures.size()));

        if (_version >= LevelVersion::Tomb4)
        {
            // Animated textures uv count - not yet used:
            activity.log("Reading animated textures UV count");
            uint8_t animated_textures_uv_count = read<uint8_t>(file);
            activity.log(std::format("Animated texture UV count: {}", animated_textures_uv_count));

            activity.log("Skipping TEX marker");
            file.seekg(3, std::ios::cur);
            if (_version == LevelVersion::Tomb5)
            {
                activity.log("Skipping TEX null terminator");
                skip(file, 1);
            }
        }

        if (get_version() == LevelVersion::Tomb3)
        {
            activity.log("Reading object textures");
            _object_textures = read_vector<uint32_t, tr_object_texture>(file);
            activity.log(std::format("Read {} object textures", _object_textures.size()));
        }
        if (get_version() == LevelVersion::Tomb4)
        {
            activity.log("Reading object textures");
            _object_textures = convert_object_textures(read_vector<uint32_t, tr4_object_texture>(file));
            activity.log(std::format("Read {} object textures", _object_textures.size()));
        }
        else if (get_version() == LevelVersion::Tomb5)
        {
            activity.log("Reading object textures");
            _object_textures = convert_object_textures(read_vector<uint32_t, tr5_object_texture>(file));
            activity.log(std::format("Read {} object textures", _object_textures.size()));
        }

        activity.log("Reading entities");
        if (_version == LevelVersion::Tomb1)
        {
            _entities = convert_entities(read_vector<uint32_t, tr_entity>(file));
        }
        else
        {
            // TR4 entity is in here, OCB is not set but goes into intensity2 (convert later).
            _entities = read_vector<uint32_t, tr2_entity>(file);
        }
        activity.log(std::format("Read {} entities", _entities.size()));

        if (_version < LevelVersion::Tomb4)
        {
            activity.log("Reading light map");
            std::vector<uint8_t> light_map = read_vector<uint8_t>(file, 32 * 256);
            activity.log("Read light map");
        }

        if (_version == LevelVersion::Tomb1)
        {
            activity.log("Reading 8-bit palette");
            _palette = read_vector<tr_colour>(file, 256);
            activity.log("Read 8-bit palette");
        }

        if (_version >= LevelVersion::Tomb4)
        {
            activity.log("Reading AI objects");
            _ai_objects = read_vector<uint32_t, tr4_ai_object>(file);
            activity.log(std::format("Read {} AI objects", _ai_objects.size()));
        }

        if (_version < LevelVersion::Tomb4)
        {
            activity.log("Reading cinematic frames");
            std::vector<tr_cinematic_frame> cinematic_frames = read_vector<uint16_t, tr_cinematic_frame>(file);
            activity.log(std::format("Read {} cinematic frames", cinematic_frames.size()));
        }

        activity.log("Reading demo data");
        std::vector<uint8_t> demo_data = read_vector<uint16_t, uint8_t>(file);
        activity.log(std::format("Read {} demo data", demo_data.size()));

        activity.log("Reading sound map");
        if (_version == LevelVersion::Tomb1)
        {
            std::vector<int16_t> sound_map = read_vector<int16_t>(file, 256);
        }
        else if (_version < LevelVersion::Tomb4)
        {
            std::vector<int16_t> sound_map = read_vector<int16_t>(file, 370);
        }
        else if (_version == LevelVersion::Tomb4)
        {
            if (demo_data.size() == 2048)
            {
                std::vector<int16_t> sound_map = read_vector<int16_t>(file, 1024);
            }
            else
            {
                std::vector<int16_t> sound_map = read_vector<int16_t>(file, 370);
            }
        }
        else
        {
            std::vector<int16_t> sound_map = read_vector<int16_t>(file, 450);
        }
        activity.log("Read sound map");

        activity.log("Reading sound details");
        std::vector<tr3_sound_details> sound_details = read_vector<uint32_t, tr3_sound_details>(file);
        activity.log(std::format("Read {} sound details", sound_details.size()));

        if (_version == LevelVersion::Tomb1)
        {
            activity.log("Reading sound data");
            std::vector<uint8_t> sound_data = read_vector<int32_t, uint8_t>(file);
            activity.log(std::format("Read {} sound data", sound_data.size()));
        }

        activity.log("Reading sample indices");
        std::vector<uint32_t> sample_indices = read_vector<uint32_t, uint32_t>(file);
        activity.log(std::format("Read {} sample indices", sample_indices.size()));
    }

    bool Level::find_first_entity_by_type(int16_t type, tr2_entity& entity) const
    {
        auto found = std::find_if(_entities.begin(), _entities.end(), [type](const auto& e) { return e.TypeID == type; });
        if (found == _entities.end())
        {
            return false;
        }
        entity = *found;
        return true;
    }

    int16_t Level::get_mesh_from_type_id(int16_t type) const
    {
        if (type != 0 || _version < LevelVersion::Tomb3)
        {
            return type;
        }

        if (_version > trlevel::LevelVersion::Tomb3)
        {
            return LaraSkinPostTR3;
        }
        return LaraSkinTR3;
    }

    std::string Level::name() const
    {
        return _name;
    }
}
