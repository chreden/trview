#include "Level.h"
#include "Level_common.h"
#include "LevelLoadException.h"

#include <ranges>
#include <spanstream>
#include <filesystem>
#include <set>
#include <map>

namespace trlevel
{
    namespace
    {
        template <uint32_t size>
        std::string read_string(std::basic_ispanstream<uint8_t>& file)
        {
            std::array<char, size> data;
            file.read(reinterpret_cast<uint8_t*>(&data[0]), size);
            return data | std::ranges::to<std::string>();
        }

        std::string read_tag_name(std::basic_ispanstream<uint8_t>& file)
        {
            auto result = read_string<8>(file);
            result.erase(result.find_last_not_of('\0') + 1);
            result.erase(result.find_last_not_of(' ') + 1);
            return result;
        }

        uint32_t convert_saturn_palette(uint16_t t)
        {
            uint16_t b = (t & 0x7c00) >> 10;
            uint16_t g = (t & 0x03e0) >> 5;
            uint16_t r = t & 0x001f;

            r = static_cast<uint16_t>((r / 31.0f) * 255.0f);
            g = static_cast<uint16_t>((g / 31.0f) * 255.0f);
            b = static_cast<uint16_t>((b / 31.0f) * 255.0f);

            if ((r == 222 && b == 238 && g == 0) ||
                (r == 0 && b == 0 && g == 0))
            {
                return 0x00000000;
            }

            uint16_t a = t & 0x8000 ? 0xff : 0x00;
            return a << 24 | b << 16 | g << 8 | r;
        }

        uint32_t convert_object_saturn_palette(uint16_t t)
        {
            uint16_t r = (t & 0x7c00) >> 10;
            uint16_t g = (t & 0x03e0) >> 5;
            uint16_t b = t & 0x001f;

            r = static_cast<uint16_t>((r / 31.0f) * 255.0f);
            g = static_cast<uint16_t>((g / 31.0f) * 255.0f);
            b = static_cast<uint16_t>((b / 31.0f) * 255.0f);

            if (r == 222 && b == 238 && g == 0)
            {
                return 0x00000000;
            }

            return 0xff000000 | b << 16 | g << 8 | r;
        }

        struct Tag
        {
            char     name[8];
            uint32_t size;
        };

        struct tr_room_light_saturn
        {
            int32_t  x;
            int32_t  y;
            int32_t  z;
            uint16_t intensity;
            uint16_t intensity2;
            uint32_t fade;
        };

        struct tr_room_staticmesh_saturn
        {
            tr_room_staticmesh mesh;
            uint16_t padding;
        };

        struct tr_entity_saturn
        {
            tr_entity entity;
            uint16_t  padding;
        };

        struct tr_room_texture_saturn
        {
            uint16_t start;
            uint16_t a2;
            uint16_t a3;
            uint16_t a4;
            uint16_t a5;
            uint16_t size;
            uint8_t a7_1;
            uint8_t subdiv_height;
            uint16_t a8;
        };

        static_assert(sizeof(tr_room_texture_saturn) == 16);

        struct tr_object_texture_saturn
        {
            uint16_t start;
            uint16_t cut0;
            uint16_t cut1;
            uint16_t cut2;
            uint16_t cut3;
            uint16_t size;
            uint8_t  width;
            uint8_t  height;
            uint16_t a8;
        };

        static_assert(sizeof(tr_object_texture_saturn) == 16);

        struct tr_sprite_texture_saturn   // 16 bytes
        {
            uint16_t start;
            uint16_t end;
            uint8_t a2_1;
            uint8_t height;
            int16_t LeftSide;
            int16_t TopSide;
            int16_t RightSide;
            int16_t BottomSide;
            int16_t a7;
        };

        static_assert(sizeof(tr_sprite_texture_saturn) == 16);

        enum class Primitive : uint16_t
        {
            TransparentTriangle = 32,
            TransparentRectangle,
            InvisibleTriangle,
            InvisibleRectangle,
            TexturedTriangle,
            TexturedRectangle,
            Sprite = 39
        };

        std::vector<tr_x_room_light> convert_lights(std::vector<tr_room_light_saturn> lights)
        {
            return lights 
                | std::views::transform([](auto&& l) -> tr_x_room_light { return { .level_version = LevelVersion::Tomb1, .tr1 = { .x = l.x, .y = l.y, .z = l.z, .intensity = l.intensity, .fade = l.fade } }; })
                | std::ranges::to<std::vector>();
        }

        std::vector<tr3_room_staticmesh> convert_room_static_meshes(std::vector<tr_room_staticmesh_saturn> meshes)
        {
            return meshes
                | std::views::transform([](const auto& mesh) -> tr3_room_staticmesh
                    {
                        return { mesh.mesh.x, mesh.mesh.y, mesh.mesh.z, mesh.mesh.rotation, 0xffff, 0, mesh.mesh.mesh_id };
                    })
                | std::ranges::to<std::vector>();
        }

        std::vector<tr2_entity> convert_entities(std::vector<tr_entity_saturn> entities)
        {
            return entities
                | std::views::transform([](const auto& e) -> tr2_entity
                    {
                        return { .TypeID = e.entity.TypeID, .Room = e.entity.Room, .x = e.entity.x, .y = e.entity.y, .z = e.entity.z, .Angle = e.entity.Angle, .Intensity1 = e.entity.Intensity1, .Intensity2 = e.entity.Intensity1, .Flags = e.entity.Flags };
                    })
                | std::ranges::to<std::vector>();
        }

        template <typename T>
        T to_le(const T& value)
        {
            static_assert(std::is_fundamental_v<T>, "Type needs to_le specialization");
            return std::byteswap(value);
        }

        template <typename U>
        std::vector<U> to_le(const std::vector<U>& value)
        {
            return value | std::views::transform([](auto&& v) { return to_le(v); }) | std::ranges::to<std::vector>();
        }

        template <>
        tr_vertex to_le(const tr_vertex& value)
        {
            return { .x = std::byteswap(value.x), .y = std::byteswap(value.y), .z = std::byteswap(value.z) };
        }

        template <>
        tr_room_portal to_le(const tr_room_portal& value)
        {
            tr_room_portal result{ .adjoining_room = to_le(value.adjoining_room), .normal = to_le(value.normal) };
            result.vertices[0] = to_le(value.vertices[0]);
            result.vertices[1] = to_le(value.vertices[1]);
            result.vertices[2] = to_le(value.vertices[2]);
            result.vertices[3] = to_le(value.vertices[3]);
            return result;
        }

        template <>
        tr_room_sector to_le(const tr_room_sector& value)
        {
            return { .floordata_index = to_le(value.floordata_index), .box_index = to_le(value.box_index), .room_below = to_le(value.room_below), .floor = to_le(value.floor), .room_above = to_le(value.room_above), .ceiling = to_le(value.ceiling) };
        }

        template <>
        tr_room_staticmesh_saturn to_le(const tr_room_staticmesh_saturn& value)
        {
            return
            {
                .mesh = 
                {
                    .x = to_le(value.mesh.x),
                    .y = to_le(value.mesh.y),
                    .z = to_le(value.mesh.z),
                    .rotation = to_le(value.mesh.rotation),
                    .intensity = to_le(value.mesh.intensity),
                    .mesh_id = to_le(value.mesh.mesh_id)
                }
            };
        }

        template <>
        tr_room_light_saturn to_le(const tr_room_light_saturn& value)
        {
            return
            {
                .x = to_le(value.x),
                .y = to_le(value.y),
                .z = to_le(value.z),
                .intensity = to_le(value.intensity),
                .intensity2 = to_le(value.intensity2),
                .fade = to_le(value.fade)
            };
        }

        template <>
        tr_camera to_le(const tr_camera& value)
        {
            return { .x = std::byteswap(value.x), .y = std::byteswap(value.y), .z = std::byteswap(value.z), .Room = value.Room, .Flag = value.Flag };
        }

        template <>
        tr_entity_saturn to_le(const tr_entity_saturn& value)
        {
            return { .entity = {.TypeID = to_le(value.entity.TypeID), .Room = to_le(value.entity.Room), .x = to_le(value.entity.x), .y = to_le(value.entity.y), .z = to_le(value.entity.z), .Angle = to_le(value.entity.Angle), .Intensity1 = to_le(value.entity.Intensity1), .Flags = to_le(value.entity.Flags) } };
        }

        template <>
        tr_room_texture_saturn to_le(const tr_room_texture_saturn& value)
        {
            return {
                .start = to_le(value.start),
                .a2 = to_le(value.a2),
                .a3 = to_le(value.a3),
                .a4 = to_le(value.a4),
                .a5 = to_le(value.a5),
                .size = to_le(value.size),
                .a7_1 = to_le(value.a7_1),
                .subdiv_height = to_le(value.subdiv_height),
                .a8 = to_le(value.a8),
            };
        }

        template <>
        tr_object_texture_saturn to_le(const tr_object_texture_saturn& value)
        {
            return {
                .start = to_le(value.start),
                .cut0 = to_le(value.cut0),
                .cut1 = to_le(value.cut1),
                .cut2 = to_le(value.cut2),
                .cut3 = to_le(value.cut3),
                .size = to_le(value.size),
                .width = to_le(value.width),
                .height = to_le(value.height),
                .a8 = to_le(value.a8),
            };
        }

        template <>
        tr_face4 to_le(const tr_face4& value)
        {
            tr_face4 face;
            face.vertices[0] = to_le(value.vertices[0]);
            face.vertices[1] = to_le(value.vertices[1]);
            face.vertices[2] = to_le(value.vertices[2]);
            face.vertices[3] = to_le(value.vertices[3]);
            face.texture = to_le(value.texture);
            return face;
        }

        template <>
        tr_face3 to_le(const tr_face3& value)
        {
            tr_face3 face;
            face.vertices[0] = to_le(value.vertices[0]);
            face.vertices[1] = to_le(value.vertices[1]);
            face.vertices[2] = to_le(value.vertices[2]);
            face.texture = to_le(value.texture);
            return face;
        }

        template <>
        tr_room_vertex to_le(const tr_room_vertex& value)
        {
            return { .vertex = to_le(value.vertex), .lighting = to_le(value.lighting) };
        }

        template <>
        tr_room_sprite to_le(const tr_room_sprite& value)
        {
            return { .vertex = to_le(value.vertex), .texture = to_le(value.texture) };
        };

        template <>
        Primitive to_le(const Primitive& value)
        {
            return static_cast<Primitive>(std::byteswap(static_cast<uint16_t>(value)));
        }

        template <>
        tr_model to_le(const tr_model& value)
        {
            return
            {
                .ID = to_le(value.ID),
                .NumMeshes = to_le(value.NumMeshes),
                .StartingMesh = to_le(value.StartingMesh),
                .MeshTree = to_le(value.MeshTree),
                .FrameOffset = to_le(value.FrameOffset),
                .Animation = to_le(value.Animation)
            };
        }

        template <>
        tr_bounding_box to_le(const tr_bounding_box& value)
        {
            return
            {
                .MinX = to_le(value.MinX),
                .MaxX = to_le(value.MaxX),
                .MinY = to_le(value.MinY),
                .MaxY = to_le(value.MaxY),
                .MinZ = to_le(value.MinZ),
                .MaxZ = to_le(value.MaxZ)
            };
        }

        template <>
        tr_staticmesh to_le(const tr_staticmesh& value)
        {
            return
            {
                .ID = to_le(value.ID),
                .Mesh = to_le(value.Mesh),
                .VisibilityBox = to_le(value.VisibilityBox),
                .CollisionBox = to_le(value.CollisionBox),
                .Flags = to_le(value.Flags)
            };
        }

        template<>
        tr_sprite_texture to_le(const tr_sprite_texture& value)
        {
            return
            {
                .Tile = to_le(value.Tile),
                .x = to_le(value.x),
                .y = to_le(value.y),
                .Width = to_le(value.Width),
                .Height = to_le(value.Height),
                .LeftSide = to_le(value.LeftSide),
                .TopSide = to_le(value.TopSide),
                .RightSide = to_le(value.RightSide),
                .BottomSide = to_le(value.BottomSide)
            };
        }

        template<>
        tr_sprite_texture_saturn to_le(const tr_sprite_texture_saturn& value)
        {
            return
            {
                .start = to_le(value.start),
                .end = to_le(value.end),
                .a2_1 = to_le(value.a2_1),
                .height = to_le(value.height),
                .LeftSide = to_le(value.LeftSide),
                .TopSide = to_le(value.TopSide),
                .RightSide = to_le(value.RightSide),
                .BottomSide = to_le(value.BottomSide),
                .a7 = to_le(value.a7),
            };
        }

        template <>
        tr_sprite_sequence to_le(const tr_sprite_sequence& value)
        {
            return { .SpriteID = to_le(value.SpriteID), .NegativeLength = to_le(value.NegativeLength), .Offset = to_le(value.Offset) };
        }

        void load_saturn_tagfile(
            std::basic_ispanstream<uint8_t>& file,
            const std::unordered_map<std::string, std::function<void(std::basic_ispanstream<uint8_t>&)>>& loader_functions,
            const std::string& end_tag)
        {
            file.seekg(0);
            read_tag_name(file);
            skip(file, 8);

            auto tag = read_tag_name(file);
            auto reader = loader_functions.find(tag);
            while (reader != loader_functions.end())
            {
                reader->second(file);
                tag = read_tag_name(file);
                if (tag == end_tag)
                {
                    break;
                }
                reader = loader_functions.find(tag);
            }
        }


        void load_saturn_tagfile(
            trview::IFiles& files,
            const std::filesystem::path& path,
            const std::unordered_map<std::string, std::function<void(std::basic_ispanstream<uint8_t>&)>>& loader_functions,
            const std::string& end_tag)
        {
            auto bytes = files.load_file(path);
            if (!bytes.has_value())
            {
                throw LevelLoadException();
            }

            const auto& bytes_value = *bytes;
            std::basic_ispanstream<uint8_t> file{ { bytes_value } };
            file.exceptions(std::ios::failbit);

            load_saturn_tagfile(file, loader_functions, end_tag);
        }

        // Room functions
        void read_roomnumb(std::basic_ispanstream<uint8_t>& file, trview::Activity&, tr3_room&)
        {
            skip(file, 8); // size, number - not needed
        }

        void read_meshpos(std::basic_ispanstream<uint8_t>& file, trview::Activity&, tr3_room& room)
        {
            room.info.x = to_le(read<int32_t>(file));
            room.info.z = to_le(read<int32_t>(file));
            room.info.yBottom = to_le(read<int32_t>(file));
            room.info.yTop = to_le(read<int32_t>(file));
        }

        void read_meshsize(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, tr3_room& room)
        {
            uint32_t something = to_le(read<uint32_t>(file));
            if (something == 2) // mesh data
            {
                const uint32_t num_data_words = to_le(read<uint32_t>(file));
                const uint32_t start = static_cast<uint32_t>(file.tellg());

                log_file(activity, file, "Reading vertices");
                uint16_t vertex_count = to_le(read<uint16_t>(file));
                room.data.vertices = convert_vertices(to_le(read_vector<tr_room_vertex>(file, vertex_count)));
                log_file(activity, file, std::format("Read {} vertices", room.data.vertices.size()));

                uint16_t num_primitives = to_le(read<uint16_t>(file));
                num_primitives;

                uint16_t num_primitive_groups = to_le(read<uint16_t>(file));

                for (uint16_t p = 0; p < num_primitive_groups; ++p)
                {
                    auto primitive_type = to_le(read<Primitive>(file));
                    switch (primitive_type)
                    {
                        case Primitive::InvisibleTriangle:
                        {
                            log_file(activity, file, "Skipping invisible triangles");
                            uint16_t triangle_count = to_le(read<uint16_t>(file));
                            skip(file, triangle_count * sizeof(tr_face3));
                            log_file(activity, file, std::format("Skipped {} invisible triangles", triangle_count));
                            break;
                        }
                        case Primitive::InvisibleRectangle:
                        {
                            log_file(activity, file, "Skipping invisible rectangles");
                            uint16_t rectangle_count = to_le(read<uint16_t>(file));
                            skip(file, rectangle_count * sizeof(tr_face4));
                            log_file(activity, file, std::format("Skipped {} invisible rectangles", rectangle_count));
                            break;
                        }
                        case Primitive::TexturedTriangle:
                        {
                            log_file(activity, file, "Reading triangles");
                            uint16_t triangle_count = to_le(read<uint16_t>(file));
                            auto new_triangles = convert_triangles(to_le(read_vector<tr_face3>(file, triangle_count)));
                            for (auto& t : new_triangles)
                            {
                                t.vertices[0] >>= 4;
                                t.vertices[1] >>= 4;
                                t.vertices[2] >>= 4;
                                t.texture >>= 4;
                            }
                            room.data.triangles.append_range(new_triangles);
                            log_file(activity, file, std::format("Read {} triangles", triangle_count));
                            break;
                        }
                        case Primitive::TransparentRectangle:
                        case Primitive::TexturedRectangle:
                        {
                            log_file(activity, file, "Reading textured rectangles");
                            uint16_t rectangle_count = to_le(read<uint16_t>(file));
                            auto new_rectangles = convert_rectangles(to_le(read_vector<tr_face4>(file, rectangle_count)));
                            for (auto& r : new_rectangles)
                            {
                                r.vertices[0] >>= 4;
                                r.vertices[1] >>= 4;
                                r.vertices[2] >>= 4;
                                r.vertices[3] >>= 4;
                                r.texture >>= 4;
                            }
                            room.data.rectangles.append_range(new_rectangles);
                            log_file(activity, file, std::format("Read {} textured rectangles", rectangle_count));
                            break;
                        }
                        case Primitive::Sprite:
                        {
                            uint16_t sprite_count = to_le(read<uint16_t>(file));
                            log_file(activity, file, "Reading sprites");
                            auto new_sprites = to_le(read_vector<tr_room_sprite>(file, sprite_count));
                            for (auto& s : new_sprites)
                            {
                                s.vertex  >>= 4;
                                s.texture >>= 4;
                            }
                            room.data.sprites.append_range(new_sprites);
                            log_file(activity, file, std::format("Read {} sprites", sprite_count));
                            break;
                        }
                    }
                }

                file.seekg(start + num_data_words * 2);
            }
            else // static meshes
            {
                log_file(activity, file, "Reading static meshes");
                const uint32_t num_static_meshes = to_le(read<uint32_t>(file));
                room.static_meshes = convert_room_static_meshes(to_le(read_vector<tr_room_staticmesh_saturn>(file, num_static_meshes)));
                log_file(activity, file, std::format("Read {} static meshes", room.static_meshes.size()));
            }
        }

        void read_doordata(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, tr3_room& room)
        {
            skip(file, 4);
            log_file(activity, file, "Reading portals");
            uint32_t num_portals = to_le(read<uint32_t>(file));
            room.portals = to_le(read_vector<tr_room_portal>(file, num_portals));
            log_file(activity, file, std::format("Read {} portals", room.portals.size()));
        }

        void read_floordat(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, tr3_room& room)
        {
            log_file(activity, file, "Reading number of z sectors");
            room.num_z_sectors = static_cast<uint16_t>(to_le(read<uint32_t>(file)));
            log_file(activity, file, std::format("There are {} z sectors", room.num_z_sectors));
            log_file(activity, file, "Reading number of x sectors");
            room.num_x_sectors = static_cast<uint16_t>(to_le(read<uint32_t>(file)));
            log_file(activity, file, std::format("There are {} x sectors", room.num_x_sectors));

            const auto floorsiz_tag = read_tag_name(file);
            uint32_t floorsize_u1 = to_le(read<uint32_t>(file));
            uint32_t floorsize_u2 = to_le(read<uint32_t>(file));
            floorsize_u1;
            floorsize_u2;

            log_file(activity, file, std::format("Reading {} sectors", room.num_z_sectors * room.num_x_sectors));
            room.sector_list = to_le(read_vector<tr_room_sector>(file, room.num_z_sectors * room.num_x_sectors));
            log_file(activity, file, std::format("Read {} sectors", room.sector_list.size()));
        }

        void read_lightamb(std::basic_ispanstream<uint8_t>& file, trview::Activity&, tr3_room& room)
        {
            room.ambient_intensity_1 = static_cast<uint16_t>(to_le(read<uint32_t>(file)));
            skip(file, 4); // unknown
        }

        void read_lightsiz(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, tr3_room& room)
        {
            skip(file, 4);
            uint32_t num_lights = to_le(read<uint32_t>(file));
            log_file(activity, file, "Reading lights");
            room.lights = convert_lights(to_le(read_vector<tr_room_light_saturn>(file, num_lights)));
            log_file(activity, file, std::format("Read {} lights", room.lights.size()));
        }

        void read_rm_flip(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, tr3_room& room)
        {
            log_file(activity, file, "Reading alternate room");
            skip(file, 4); // unknown
            room.alternate_room = static_cast<int16_t>(to_le(read<uint32_t>(file)));
            log_file(activity, file, std::format("Read alternate room: {}", room.alternate_room));
        }

        void read_rm_flags(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, tr3_room& room)
        {
            log_file(activity, file, "Reading flags");
            skip(file, 4); // unknown
            room.flags = static_cast<int16_t>(to_le(read<uint32_t>(file)));
            log_file(activity, file, std::format("Read flags: {:X}", room.flags));
        }

        std::vector<tr3_room> read_roomdata(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity)
        {
            skip(file, 4);
            uint32_t num_rooms = to_le(read<uint32_t>(file));

            const std::unordered_map<std::string, std::function<void(std::basic_ispanstream<uint8_t>&, trview::Activity& activity, tr3_room&)>> room_functions
            {
                { "ROOMNUMB", read_roomnumb },
                { "MESHPOS", read_meshpos },
                { "MESHSIZE", read_meshsize },
                { "DOORDATA", read_doordata },
                { "FLOORDAT", read_floordat },
                { "LIGHTAMB", read_lightamb },
                { "LIGHTSIZ", read_lightsiz },
                { "RM_FLIP", read_rm_flip },
                { "RM_FLAGS", read_rm_flags }
            };

            std::vector<tr3_room> rooms;
            for (uint32_t r = 0; r < num_rooms; ++r)
            {
                tr3_room room{};

                auto tag = read_tag_name(file);
                auto reader = room_functions.find(tag);
                while (reader != room_functions.end())
                {
                    reader->second(file, activity, room);
                    if (reader->first == "RM_FLAGS")
                    {
                        break;
                    }
                    tag = read_tag_name(file);
                    reader = room_functions.find(tag);
                }
                rooms.push_back(room);
            }
            return rooms;
        }
    }

    void Level::load_tr1_saturn(std::basic_ispanstream<uint8_t>& level_file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        std::vector<tr_room_texture_saturn> object_textures;
        std::vector<uint8_t> all_object_texture_data;
        std::vector<uint8_t> tqtr_data;

        const auto read_roomtinf = [&](auto& file)
        {
            uint32_t roomtinf_size = to_le(read<uint32_t>(file));
            roomtinf_size;
            uint32_t roomtinf_count = to_le(read<uint32_t>(file));
            object_textures = to_le(read_vector<tr_room_texture_saturn>(file, roomtinf_count));
        };

        const auto read_roomtqtr = [&](auto& file)
        {
            skip(file, 4);
            uint32_t count = to_le(read<uint32_t>(file));
            tqtr_data = read_vector<uint8_t>(file, count);
        };

        const auto read_roomtsub = [&](auto& file)
        {
            skip(file, 4);
            uint32_t roomtsub_count = to_le(read<uint32_t>(file));
            all_object_texture_data = read_vector<uint8_t>(file, roomtsub_count);
        };

        const auto read_roomdata_forward = [&](auto& file)
        {
            _rooms = read_roomdata(file, activity);
        };

        const auto read_flordata = [&](auto& file)
        {
            skip(file, 4);
            uint32_t num_floor_data = to_le(read<uint32_t>(file));
            callbacks.on_progress("Reading floor data");
            log_file(activity, file, "Reading floor data");
            _floor_data = to_le(read_vector<uint16_t>(file, num_floor_data));
            log_file(activity, file, std::format("Read {} floor data", _floor_data.size()));
        };

        const auto read_cameras = [&](auto& file)
        {
            skip(file, 4);
            uint32_t num_cameras = to_le(read<uint32_t>(file));
            callbacks.on_progress("Reading cameras");
            log_file(activity, file, "Reading cameras");
            _cameras = to_le(read_vector<tr_camera>(file, num_cameras));
            log_file(activity, file, std::format("Read {} cameras", _cameras.size()));
        };

        const auto read_soundfx = [&](auto& file)
        {
            uint32_t soundfx_size = to_le(read<uint32_t>(file));
            uint32_t soundfx_count = to_le(read<uint32_t>(file));
            skip(file, soundfx_size * soundfx_count);
        };

        const auto read_generic = [&](auto& file)
        {
            uint32_t generic_size = to_le(read<uint32_t>(file));
            uint32_t generic_count = to_le(read<uint32_t>(file));
            skip(file, generic_size * generic_count);
        };

        const auto read_itemdata = [&](auto& file)
        {
            callbacks.on_progress("Reading entities");
            log_file(activity, file, "Reading entities");
            uint32_t itemdata_size = to_le(read<uint32_t>(file));
            itemdata_size;
            uint32_t itemdata_count = to_le(read<uint32_t>(file));
            _entities = convert_entities(to_le(read_vector<tr_entity_saturn>(file, itemdata_count)));
            log_file(activity, file, std::format("Read {} entities", _entities.size()));
        };

        const std::unordered_map<std::string, std::function<void(std::basic_ispanstream<uint8_t>&)>> loader_functions
        {
            { "ROOMTINF", read_roomtinf },
            { "ROOMTQTR", read_roomtqtr },
            { "ROOMTSUB", read_roomtsub },
            { "ROOMTPAL", read_generic },
            { "ROOMSPAL", read_generic },
            { "ROOMDATA", read_roomdata_forward },
            { "FLORDATA", read_flordata },
            { "CAMERAS",  read_cameras },
            { "SOUNDFX",  read_soundfx },
            { "BOXES",    read_generic },
            { "OVERLAPS", read_generic },
            { "GND_ZONE", read_generic },
            { "GND_ZON2", read_generic },
            { "FLY_ZONE", read_generic },
            { "ARANGES",  read_generic },
            { "ITEMDATA", read_itemdata }
        };

        load_saturn_tagfile(level_file, loader_functions, "ROOMEND");
        load_tr1_saturn_sad(activity, callbacks);

        for (auto& room : _rooms)
        {
            for (auto& face : room.data.rectangles)
            {
                face.texture += static_cast<uint16_t>(_object_textures.size());
            }

            for (auto& face : room.data.triangles)
            {
                face.texture += static_cast<uint16_t>(_object_textures.size());
            }
        }

        for (const auto object_texture : object_textures)
        {
            const auto start = object_texture.a2 << 3;
            const auto size = ((object_texture.a5 + object_texture.size) << 3) - start;
            const auto end = start + size;

            const auto data =
                std::ranges::subrange(all_object_texture_data.begin() + start, all_object_texture_data.begin() + end)
                | std::ranges::to<std::vector>();

            const auto palette_bytes =
                std::ranges::subrange(tqtr_data.begin() + (object_texture.start * 8 + object_texture.size * 8), tqtr_data.begin() + (object_texture.start * 8 + object_texture.size * 8) + 32)
                | std::ranges::to<std::vector>();

            std::array<uint16_t, 16> palette;
            memcpy(&palette[0], &palette_bytes[0], sizeof(uint16_t) * 16);

            std::vector<uint8_t> object_texture_8;
            object_texture_8.resize(256 * 256);

            const uint32_t subdiv_height = object_texture.subdiv_height;
            const uint32_t width_pixels = static_cast<uint32_t>(data.size()) / subdiv_height;
            const uint32_t subdiv_width_bytes = width_pixels / 4;
            const uint32_t height_pixels = subdiv_height * 2;

            for (uint32_t y = 0; y < subdiv_height; ++y)
            {
                // Top left.
                memcpy(&object_texture_8[y * 256], &data[y * subdiv_width_bytes], subdiv_width_bytes);
                // Top right
                memcpy(&object_texture_8[y * 256 + subdiv_width_bytes], &data[(y + subdiv_height) * subdiv_width_bytes], subdiv_width_bytes);
                // Bottom left
                memcpy(&object_texture_8[(y + subdiv_height) * 256], &data[(y + subdiv_height * 3) * subdiv_width_bytes], subdiv_width_bytes);
                // Bottom right
                memcpy(&object_texture_8[(y + subdiv_height) * 256 + subdiv_width_bytes], &data[(y + subdiv_height * 2) * subdiv_width_bytes], subdiv_width_bytes);
            }

            std::vector<uint32_t> object_texture_data;
            object_texture_data.resize(256 * 256);

            for (uint32_t y = 0; y < 256; ++y)
            {
                for (uint32_t x = 0; x < 128; ++x)
                {
                    uint8_t value1 = ((object_texture_8[y * 256 + x] & 0xF0) >> 4);
                    uint8_t value2 = (object_texture_8[y * 256 + x] & 0xF);

                    uint16_t pe1 = to_le(palette[value1]);
                    uint16_t pe2 = to_le(palette[value2]);

                    object_texture_data[y * 256 + x * 2]     = convert_saturn_palette(pe1);
                    object_texture_data[y * 256 + x * 2 + 1] = convert_saturn_palette(pe2);
                }
            }

            tr_object_texture new_object_texture
            {
                .Attribute = static_cast<uint16_t>(std::ranges::any_of(object_texture_data, [](auto&& p) { return p == 0; }) ? 1 : 0),
                .TileAndFlag = static_cast<uint16_t>(_num_textiles),
                .Vertices =
                {
                    { 0, 0, 0, 0 },
                    { 0, static_cast<uint8_t>(width_pixels), 0, 0 },
                    { 0, 0, 0, static_cast<uint8_t>(height_pixels) },
                    { 0, static_cast<uint8_t>(width_pixels), 0, static_cast<uint8_t>(height_pixels) }
                }
            };
            _object_textures.push_back(new_object_texture);

            _num_textiles++;
            callbacks.on_textile(object_texture_data);
        }

        load_tr1_saturn_spr(activity, callbacks);
        load_tr1_saturn_snd(activity, callbacks);

        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
        callbacks.on_progress("Loading complete");

        std::map<int16_t, uint32_t> ot_refs;
        for (auto& [_, mesh] : _meshes)
        {
            for (auto& r : mesh.textured_rectangles)
            {
                int16_t tex = static_cast<int16_t>(r.texture);
                r.texture = tex >> 4;
            }
        }
    }

    void Level::load_tr1_saturn_sad(trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        const auto read_anibones = [&](auto& file)
        {
            uint32_t bones_size = to_le(read<uint32_t>(file));
            bones_size;
            uint32_t bones_count = to_le(read<uint32_t>(file));
            _meshtree = to_le(read_vector<uint32_t>(file, bones_count));
        };

        const auto read_anims = [&](auto& file)
        {
            uint32_t anims_size = to_le(read<uint32_t>(file));
            anims_size;
            uint32_t anims_count = to_le(read<uint32_t>(file));
            skip(file, anims_count * sizeof(tr_animation));
        };

        const auto read_generic = [&](auto& file)
        {
            uint32_t size = to_le(read<uint32_t>(file));
            uint32_t count = to_le(read<uint32_t>(file));
            skip(file, size * count);
        };

        const auto read_meshdata = [&](auto& file)
        {
            skip(file, 4);
            uint32_t count = to_le(read<uint32_t>(file));
            _mesh_data = read_vector<uint16_t>(file, count);
        };

        const auto read_meshptrs = [&](auto& file)
        {
            skip(file, 4);
            uint32_t count = to_le(read<uint32_t>(file));
            _mesh_pointers = to_le(read_vector<uint32_t>(file, count));
        };

        const auto read_frames = [&](auto& file)
        {
            skip(file, 4);
            uint32_t count = to_le(read<uint32_t>(file));
            _frames = to_le(read_vector<uint16_t>(file, count));
        };

        const auto read_animobj = [&](auto& file)
        {
            skip(file, 4);
            uint32_t count = to_le(read<uint32_t>(file));
            for (uint32_t i = 0; i < count; ++i)
            {
                auto model = to_le(read<tr_model>(file));
                _models.push_back(model);
                skip(file, 2);
            }
        };

        const auto read_statobj = [&](auto& file)
        {
            callbacks.on_progress("Reading static meshes");
            log_file(activity, file, "Reading static meshes");
            skip(file, 4);
            uint32_t count = to_le(read<uint32_t>(file));
            const auto static_meshes = to_le(read_vector<tr_staticmesh>(file, count));
            log_file(activity, file, std::format("Read {} static meshes", static_meshes.size()));
            for (const auto& mesh : static_meshes)
            {
                _static_meshes.insert({ mesh.ID, mesh });
            }
        };

        std::vector<tr_object_texture_saturn> object_textures;
        std::vector<uint8_t> all_object_texture_data;

        const auto read_otextinf = [&](auto& file)
        {
            skip(file, 4);
            uint32_t count = to_le(read<uint32_t>(file));
            object_textures = to_le(read_vector<tr_object_texture_saturn>(file, count));
        };

        const auto read_otextdat = [&](auto& file)
        {
            skip(file, 4);
            uint32_t count = to_le(read<uint32_t>(file));
            all_object_texture_data = read_vector<uint8_t>(file, count);
        };

        const std::unordered_map<std::string, std::function<void(std::basic_ispanstream<uint8_t>&)>> loader_functions
        {
            { "ANIMS", read_anims },
            { "CHANGES", read_generic },
            { "RANGES", read_generic },
            { "COMMANDS", read_generic },
            { "ANIBONES", read_anibones },
            { "ANIMOBJ", read_animobj },
            { "STATOBJ", read_statobj },
            { "FRAMES", read_frames },
            { "MESHPTRS", read_meshptrs },
            { "MESHDATA", read_meshdata },
            { "OTEXTINF", read_otextinf },
            { "OTEXTDAT", read_otextdat },
            { "ITEXTINF", read_generic },
            { "ITEXTDAT", read_generic },
        };

        std::filesystem::path path{ _filename };
        path.replace_extension("SAD");
        load_saturn_tagfile(*_files, path, loader_functions, "OBJEND");

        // Generate object textures:
        for (const auto& object_texture : object_textures)
        {
            std::vector<uint32_t> object_texture_data;
            object_texture_data.resize(256 * 256);

            std::vector<uint8_t> data;
            std::array<uint16_t, 16> palette;
            std::optional<int> cut;

            const auto read_cut_data = [&](auto&& offset)
            {
                const uint32_t start = offset << 3;
                const uint32_t end = start + (object_texture.size << 3);
                data = std::ranges::subrange(all_object_texture_data.begin() + start, all_object_texture_data.begin() + end)
                    | std::ranges::to<std::vector>();
                const auto palette_bytes =
                    std::ranges::subrange(all_object_texture_data.begin() + end, all_object_texture_data.begin() + end + 32)
                    | std::ranges::to<std::vector>();
                memcpy(&palette[0], &palette_bytes[0], sizeof(uint16_t) * 16);
            };

            if (object_texture.start != 1)
            {
                read_cut_data(object_texture.start);
            }
            else if (object_texture.cut0 != 1)
            {
                read_cut_data(object_texture.cut0);
                cut = 0;
            }
            else if (object_texture.cut1 != 1)
            {
                read_cut_data(object_texture.cut1);
                cut = 1;
            }
            else if (object_texture.cut2 != 1)
            {
                read_cut_data(object_texture.cut2);
                cut = 2;
            }
            else if (object_texture.cut3 != 1)
            {
                read_cut_data(object_texture.cut3);
                cut = 3;
            }

            if (!data.empty())
            {
                {
                    std::ofstream outfile;
                    outfile.open("c:\\dev\\trview-temp\\ot.bin", std::ios::binary | std::ios::out);
                    outfile.write(reinterpret_cast<const char*>(&data[0]), data.size());
                }

                uint32_t width = object_texture.width << 2;
                uint32_t height = object_texture.height;
                bool any_transparent = false;

                for (uint32_t y = 0; y < height; ++y)
                {
                    for (uint32_t x = 0; x < width; ++x)
                    {
                        uint8_t value1 = ((data[y * width + x] & 0xF0) >> 4);
                        uint8_t value2 = (data[y * width + x] & 0x0F);

                        uint16_t pe1 = to_le(palette[value1]);
                        uint16_t pe2 = to_le(palette[value2]);

                        uint32_t c1 = convert_saturn_palette(pe1);
                        uint32_t c2 = convert_saturn_palette(pe2);

                        object_texture_data[y * 256 + x * 2 + 0] = cut.has_value() ? ((value1 == 0xF) ? 0x00000000 : c1) : c1;
                        object_texture_data[y * 256 + x * 2 + 1] = cut.has_value() ? ((value2 == 0xF) ? 0x00000000 : c2) : c2;
                        any_transparent |=
                            ((object_texture_data[y * 256 + x * 2 + 0] & 0xff000000) == 0) |
                            ((object_texture_data[y * 256 + x * 2 + 1] & 0xff000000) == 0);
                    }
                }

                tr_object_texture new_object_texture
                {
                    .Attribute = any_transparent ? static_cast<uint16_t>(1) : static_cast<uint16_t>(0),
                    .TileAndFlag = static_cast<uint16_t>(_num_textiles),
                    .Vertices =
                    {
                        { 0, 0, 0, 0 },
                        { 0, static_cast<uint8_t>(object_texture.width << 3), 0, 0 },
                        { 0, 0, 0, static_cast<uint8_t>(object_texture.height) },
                        { 0, static_cast<uint8_t>(object_texture.width << 3), 0, static_cast<uint8_t>(object_texture.height) }
                    }
                };

                if (cut.has_value())
                {
                    switch (cut.value())
                    {
                        case 0:
                        {
                            // std::swap(new_object_texture.Vertices[0], new_object_texture.Vertices[3]);
                            break;
                        }
                        case 1:
                        {
                            // Eyebrows
                            std::swap(new_object_texture.Vertices[0], new_object_texture.Vertices[2]);
                            std::swap(new_object_texture.Vertices[1], new_object_texture.Vertices[3]);
                            break;
                        }
                        case 2:
                        {
                            // std::swap(new_object_texture.Vertices[0], new_object_texture.Vertices[3]);
                            // std::swap(new_object_texture.Vertices[1], new_object_texture.Vertices[2]);
                            break;
                        }
                        case 3:
                        {
                            // std::swap(new_object_texture.Vertices[0], new_object_texture.Vertices[3]);
                            break;
                        }
                    }
                }

                _object_textures.push_back(new_object_texture);

                _num_textiles++;
                callbacks.on_textile(object_texture_data);
            }
            else
            {
                tr_object_texture new_object_texture
                {
                    .Attribute = 0,
                    .TileAndFlag = static_cast<uint16_t>(_num_textiles),
                    .Vertices =
                    {
                        { 0, 0, 0, 0 },
                        { 0, static_cast<uint8_t>(object_texture.width << 3), 0, 0 },
                        { 0, 0, 0, static_cast<uint8_t>(object_texture.height) },
                        { 0, static_cast<uint8_t>(object_texture.width << 3), 0, static_cast<uint8_t>(object_texture.height) }
                    }
                };
                _num_textiles++;
                _object_textures.push_back(new_object_texture);
                callbacks.on_textile(object_texture_data);
            }
        }
        std::map<uint32_t, std::string> cuts;
        uint32_t index = 0;
        for (const auto& object_texture : object_textures)
        {
            cuts[index] = 
                std::format("{},{},{},{},{}",
                    object_texture.start << 3,
                    object_texture.cut0 << 3,
                    object_texture.cut1 << 3,
                    object_texture.cut2 << 3,
                    object_texture.cut3 << 3);
            ++index;
        }
    }

    void Level::load_tr1_saturn_snd(trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        activity;
        callbacks;

        const auto read_generic = [&](auto& file)
        {
            uint32_t size = to_le(read<uint32_t>(file));
            uint32_t count = to_le(read<uint32_t>(file));
            skip(file, size * count);
        };

        const std::unordered_map<std::string, std::function<void(std::basic_ispanstream<uint8_t>&)>> loader_functions
        {
            { "SAMPLUT", read_generic },
            { "SAMPLE", read_generic },
        };

        std::filesystem::path path{ _filename };
        path.replace_extension("SND");
        load_saturn_tagfile(*_files, path, loader_functions, "ENDFILE");
    }

    void Level::load_tr1_saturn_spr(trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        activity;
        callbacks;

        std::vector<uint8_t> spritdat;
        std::vector<tr_sprite_texture_saturn> sprite_textures;

        const auto read_spritdat = [&](auto& file)
        {
            skip(file, 4);
            uint32_t count = to_le(read<uint32_t>(file));
            spritdat = read_vector<uint8_t>(file, count);
        };

        const auto read_spritinf = [&](auto& file)
        {
            skip(file, 4);
            uint32_t count = to_le(read<uint32_t>(file));
            sprite_textures = to_le(read_vector<tr_sprite_texture_saturn>(file, count));
        };

        const auto read_objects = [&](auto& file)
        {
            skip(file, 4);
            uint32_t count = to_le(read<uint32_t>(file));
            std::vector<tr_sprite_sequence> sprite_sequences;
            for (uint32_t i = 0; i < count; ++i)
            {
                tr_sprite_sequence new_sequence{};
                new_sequence.SpriteID = to_le(read<uint32_t>(file));
                new_sequence.NegativeLength = to_le(read<int16_t>(file));
                if (new_sequence.NegativeLength > 0)
                {
                    file.seekg(-1, std::ios::cur);
                    new_sequence.NegativeLength = read<int8_t>(file);
                }
                else
                {
                    new_sequence.Offset = to_le(read<int16_t>(file));
                }
                sprite_sequences.push_back(new_sequence);
            }
            _sprite_sequences = sprite_sequences;
        };

        const std::unordered_map<std::string, std::function<void(std::basic_ispanstream<uint8_t>&)>> loader_functions
        {
            { "SPRITDAT", read_spritdat },
            { "SPRITINF", read_spritinf },
            { "OBJECTS", read_objects },
        };

        std::filesystem::path path{ _filename };
        path.replace_extension("SPR");
        load_saturn_tagfile(*_files, path, loader_functions, "SPRITEND");

        for (const auto sprite_texture : sprite_textures)
        {
            const auto data = 
                std::ranges::subrange(spritdat.begin() + (sprite_texture.start << 3), spritdat.begin() + (sprite_texture.end << 3))
              | std::ranges::to<std::vector>();
            std::vector<uint32_t> sprite_texture_data;
            sprite_texture_data.resize(256 * 256);

            const auto palette_bytes = 
                std::ranges::subrange(spritdat.begin() + (sprite_texture.end << 3), spritdat.begin() + (sprite_texture.end << 3) + 32)
              | std::ranges::to<std::vector>();

            std::array<uint16_t, 16> palette;
            memcpy(&palette[0], &palette_bytes[0], sizeof(uint16_t) * 16);

            uint32_t height = sprite_texture.height;
            uint32_t width = static_cast<uint32_t>(data.size() / height);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    uint8_t value1 = ((data[y * width + x] & 0xF0) >> 4);
                    uint8_t value2 = (data[y * width + x] & 0xF);

                    uint16_t pe1 = value1 ? to_le(palette[value1]) : 0x00000000;
                    uint16_t pe2 = value2 ? to_le(palette[value2]) : 0x00000000;

                    sprite_texture_data[y * 256 + x * 2 + 0] = convert_saturn_palette(pe1);
                    sprite_texture_data[y * 256 + x * 2 + 1] = convert_saturn_palette(pe2);
                }
            }

            tr_sprite_texture new_sprite_texture
            {
                .Tile = static_cast<uint16_t>(_num_textiles),
                .x = 0,
                .y = 0,
                .Width = static_cast<uint16_t>((width * 2 * 256) + 255),
                .Height = static_cast<uint16_t>((height * 256) + 255),
                .LeftSide = sprite_texture.LeftSide,
                .TopSide = sprite_texture.TopSide,
                .RightSide = sprite_texture.RightSide,
                .BottomSide = sprite_texture.BottomSide
            };
            _sprite_textures.push_back(new_sprite_texture);

            _num_textiles++;
            callbacks.on_textile(sprite_texture_data);
        }
    }

    void Level::generate_mesh_tr1_saturn(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream)
    {
        mesh.centre = to_le(read<tr_vertex>(stream));
        mesh.coll_radius = to_le(read<uint16_t>(stream));

        uint16_t unknown = to_le(read<uint16_t>(stream));
        unknown;

        int16_t vertices_count = to_le(read<int16_t>(stream));
        int16_t normals_count = vertices_count;
        normals_count;
        vertices_count = static_cast<int16_t>(std::abs(vertices_count));
        mesh.vertices = to_le(read_vector<tr_vertex>(stream, vertices_count));

        int16_t normals = to_le(read<int16_t>(stream));
        if (normals > 0)
        {
            mesh.normals = to_le(read_vector<tr_vertex>(stream, normals));
        }
        else
        {
            mesh.lights = to_le(read_vector<int16_t>(stream, abs(normals)));
        }

        std::vector<tr_face3> triangles;
        std::vector<tr_face4> rectangles;

        const uint16_t num_primitives = to_le(read<uint16_t>(stream));
        uint16_t total_primitives = 0;

        auto read_rects = [&]() -> std::vector<tr_face4>
        {
            if (total_primitives < num_primitives)
            {
                uint16_t trects_count = to_le(read<uint16_t>(stream));
                total_primitives += trects_count;
                auto trects = to_le(read_vector<tr_face4>(stream, trects_count));
                for (auto& r : trects)
                {
                    r.vertices[0] >>= 5;
                    r.vertices[1] >>= 5;
                    r.vertices[2] >>= 5;
                    r.vertices[3] >>= 5;
                }
                return trects;
            }
            return {};
        };

        auto read_tris = [&]() -> std::vector<tr_face3>
            {
                if (total_primitives < num_primitives)
                {
                    uint16_t ttris_count = to_le(read<uint16_t>(stream));
                    total_primitives += ttris_count;
                    auto ttris = to_le(read_vector<tr_face3>(stream, ttris_count));
                    for (auto& r : ttris)
                    {
                        r.vertices[0] >>= 5;
                        r.vertices[1] >>= 5;
                        r.vertices[2] >>= 5;
                    }
                    return ttris;
                }
                return {};
            };

        const uint16_t num_primitive_types = to_le(read<uint16_t>(stream));
        for (uint16_t p = 0; p < num_primitive_types; ++p)
        {
            const uint16_t prim_type = to_le(read<uint16_t>(stream));
            switch (prim_type)
            {
                case 4: // Coloured Tris
                {
                    auto tris = read_tris();
                    mesh.coloured_triangles.append_range(tris);
                    break;
                }
                case 5: // coloured rects
                {
                    auto rects = read_rects();
                    mesh.coloured_rectangles.append_range(rects);
                    break;
                }
                case 9:
                case 49:
                case 17:
                case 57:
                {
                    auto rects = read_rects();
                    mesh.textured_rectangles.append_range(convert_rectangles(rects));
                    break;
                }
                case 2:
                case 8:
                case 16:
                {
                    auto tris = read_tris();
                    mesh.textured_triangles.append_range(convert_triangles(tris));
                    break;
                }
            }
        }
    }
}