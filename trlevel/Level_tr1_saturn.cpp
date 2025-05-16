#include "Level.h"
#include "Level_common.h"
#include "LevelLoadException.h"

#include <trview.common/Algorithms.h>

#include <ranges>
#include <spanstream>
#include <filesystem>
#include <map>
#include <unordered_set>

#include "TileMapper.h"

namespace trlevel
{
    namespace
    {
        struct SaturnTextureInfo
        {
            struct Mapping
            {
                uint16_t index{ 0u };
                uint32_t transparent_colour{ 0u };
            };
            std::map<uint16_t, std::array<std::optional<Mapping>, 5>> object_texture_mapping;
            std::vector<std::vector<uint32_t>> textiles;
        };

        template <typename T>
        void write(std::basic_ospanstream<uint8_t>& file, const T& value)
        {
            file.write(reinterpret_cast<const uint8_t*>(&value), sizeof(value));
        }

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

        enum class MeshPrimitive : uint16_t
        {
            UnknownTriangle = 2,
            ColouredTriangle = 4,
            ColouredRectangle = 5,
            TexturedTriangle = 8,
            TexturedRectangle = 9,
            TransparentTriangle = 16,
            TransparentRectangle = 17,
            TexturedMirroredRectangle = 49,
            TexturedMirroredTransparentRectangle = 57
        };

        // Temporary storage for generated textures so transparency can be correctly mapped.
        struct ObjectTextureData
        {
            std::vector<uint32_t> pixels;
            uint32_t              width;
            uint32_t              height;
            uint32_t              transparent_colour;
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
            return { .x = std::byteswap(value.x), .y = std::byteswap(value.y), .z = std::byteswap(value.z), .Room = to_le(value.Room), .Flag = to_le(value.Flag) };
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
        MeshPrimitive to_le(const MeshPrimitive& value)
        {
            return static_cast<MeshPrimitive>(std::byteswap(static_cast<uint16_t>(value)));
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

        template <>
        tr_sound_details to_le(const tr_sound_details& value)
        {
            return { .Sample = to_le(value.Sample), .Volume = to_le(value.Volume), .Chance = to_le(value.Chance), .Characteristics = to_le(value.Characteristics) };
        }

        template <>
        tr_sound_source to_le(const tr_sound_source& value)
        {
            return { .x = to_le(value.x), .y = to_le(value.y), .z = to_le(value.z), .SoundID = to_le(value.SoundID), .Flags = to_le(value.Flags) };
        }

        constexpr uint16_t get_texture_operation(uint16_t texture)
        {
            const uint16_t raw_op = (texture & 0xE000) >> 13;
            switch (raw_op)
            {
            case 4:
                return 1;
            case 5:
                return 0;
            case 6:
                return 3;
            case 7:
                return 2;
            }
            return raw_op;
        }

        // Reads and discards a Saturn tag. Only works when the size is accurate.
        void read_generic(std::basic_ispanstream<uint8_t>& file)
        {
            uint32_t size = to_le(read<uint32_t>(file));
            uint32_t count = to_le(read<uint32_t>(file));
            skip(file, size * count);
        };

        // Reads a list of entries from a Saturn tag and stores it in an output location.
        template <typename T>
        std::function<void(std::basic_ispanstream<uint8_t>&)> read_list(std::vector<T>& out)
        {
            return [&](auto& file)
                {
                    skip(file, 4); // Discard size - not always accurate anyway.
                    uint32_t count = to_le(read<uint32_t>(file));
                    out = to_le(read_vector<T>(file, count));
                };
        }

        uint32_t load_saturn_tagfile(
            std::basic_ispanstream<uint8_t>& file,
            const std::unordered_map<std::string, std::function<void(std::basic_ispanstream<uint8_t>&)>>& loader_functions,
            const std::string& end_tag)
        {
            file.seekg(0);
            read_tag_name(file);
            skip(file, 4);
            const uint32_t version = to_le(read<uint32_t>(file));

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

            return version;
        }


        uint32_t load_saturn_tagfile(
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

            return load_saturn_tagfile(file, loader_functions, end_tag);
        }

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
                auto vertices = to_le(read_vector<tr_room_vertex>(file, vertex_count));
                for (auto& v : vertices)
                {
                    v.lighting = (-v.lighting) / 4;
                }
                room.data.vertices = convert_vertices(vertices);
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
                                r.effects = primitive_type == Primitive::TransparentRectangle ? 1 : 0;
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
        load_tr1_saturn_sad(activity, callbacks);
        load_tr1_saturn_sat(level_file, activity, callbacks);
        load_tr1_saturn_spr(activity, callbacks);

        // Some saturn levels have broken SND files.
        try
        {
            load_tr1_saturn_snd(activity, callbacks);
            generate_sounds(callbacks);
        }
        catch (...)
        {
        }
    }

    void Level::load_tr1_saturn_sat(std::basic_ispanstream<uint8_t>& level_file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        std::vector<tr_room_texture_saturn> room_textures;
        std::vector<uint8_t> all_room_texture_data;
        std::vector<uint8_t> tqtr_data;

        const auto read_roomdata_forward = [&](auto& file)
            {
                _rooms = read_roomdata(file, activity);
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
            { "ROOMTINF", read_list(room_textures) },
            { "ROOMTQTR", read_list(tqtr_data) },
            { "ROOMTSUB", read_list(all_room_texture_data) },
            { "ROOMTPAL", read_generic },
            { "ROOMSPAL", read_generic },
            { "ROOMDATA", read_roomdata_forward },
            { "FLORDATA", read_list(_floor_data) },
            { "CAMERAS",  read_list(_cameras) },
            { "SOUNDFX",  read_list(_sound_sources) },
            { "BOXES",    read_generic },
            { "OVERLAPS", read_generic },
            { "GND_ZONE", read_generic },
            { "GND_ZON2", read_generic },
            { "FLY_ZONE", read_generic },
            { "ARANGES",  read_generic },
            { "ITEMDATA", read_itemdata }
        };

        load_saturn_tagfile(level_file, loader_functions, "ROOMEND");

        std::unordered_set<uint16_t> transparent_room_object_textures;
        for (auto& room : _rooms)
        {
            for (auto& face : room.data.rectangles)
            {
                face.texture += static_cast<uint16_t>(_object_textures.size());
                if (face.effects)
                {
                    transparent_room_object_textures.insert(face.texture & 0xFFF);
                }
            }

            for (auto& face : room.data.triangles)
            {
                face.texture += static_cast<uint16_t>(_object_textures.size());
                if (face.effects)
                {
                    transparent_room_object_textures.insert(face.texture & 0xFFF);
                }
            }
        }

        TileMapper mapper(_num_textiles, [&](const std::vector<uint32_t>& data)
            {
                callbacks.on_textile(data);
                ++_num_textiles;
            });

        for (const auto room_texture : room_textures)
        {
            const auto start = room_texture.a2 << 3;
            const auto size = ((room_texture.a5 + room_texture.size) << 3) - start;
            const auto end = start + size;

            const auto data =
                std::ranges::subrange(all_room_texture_data.begin() + start, all_room_texture_data.begin() + end)
                | std::ranges::to<std::vector>();

            const auto palette_bytes =
                std::ranges::subrange(tqtr_data.begin() + (room_texture.start * 8 + room_texture.size * 8), tqtr_data.begin() + (room_texture.start * 8 + room_texture.size * 8) + 32)
                | std::ranges::to<std::vector>();

            std::array<uint16_t, 16> palette;
            memcpy(&palette[0], &palette_bytes[0], sizeof(uint16_t) * 16);

            const uint32_t subdiv_height = room_texture.subdiv_height;
            const uint32_t width_pixels = static_cast<uint32_t>(data.size()) / subdiv_height;
            const uint32_t subdiv_width_bytes = width_pixels / 4;
            const uint32_t height_pixels = subdiv_height * 2;
            const uint32_t source_width = width_pixels / 2;

            std::vector<uint8_t> object_texture_8;
            object_texture_8.resize(source_width * height_pixels);

            for (uint32_t y = 0; y < subdiv_height; ++y)
            {
                // Top left.
                memcpy(&object_texture_8[y * source_width], &data[y * subdiv_width_bytes], subdiv_width_bytes);
                // Top right
                memcpy(&object_texture_8[y * source_width + subdiv_width_bytes], &data[(y + subdiv_height) * subdiv_width_bytes], subdiv_width_bytes);
                // Bottom left
                memcpy(&object_texture_8[(y + subdiv_height) * source_width], &data[(y + subdiv_height * 3) * subdiv_width_bytes], subdiv_width_bytes);
                // Bottom right
                memcpy(&object_texture_8[(y + subdiv_height) * source_width + subdiv_width_bytes], &data[(y + subdiv_height * 2) * subdiv_width_bytes], subdiv_width_bytes);
            }

            std::vector<uint32_t> object_texture_data;
            object_texture_data.resize(width_pixels * height_pixels);

            const bool is_transparent =
                transparent_room_object_textures.find(static_cast<uint16_t>(_object_textures.size())) !=
                transparent_room_object_textures.end();

            for (uint32_t y = 0; y < height_pixels; ++y)
            {
                for (uint32_t x = 0; x < source_width; ++x)
                {
                    uint8_t value1 = ((object_texture_8[y * source_width + x] & 0xF0) >> 4);
                    uint8_t value2 = (object_texture_8[y * source_width + x] & 0xF);

                    uint32_t pe1 = convert_saturn_palette(to_le(palette[value1]));
                    uint32_t pe2 = convert_saturn_palette(to_le(palette[value2]));

                    if (is_transparent)
                    {
                        if (value1 == 0x0) { pe1 = 0x00000000; }
                        if (value2 == 0x0) { pe2 = 0x00000000; }
                    }

                    object_texture_data[y * width_pixels + x * 2] = pe1;
                    object_texture_data[y * width_pixels + x * 2 + 1] = pe2;
                }
            }

            _object_textures.push_back(mapper.map(object_texture_data, width_pixels, height_pixels));
        }

        mapper.finish();
    }

    void Level::load_tr1_saturn_sad(trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        const auto read_anims = [&](auto& file)
        {
            uint32_t anims_size = to_le(read<uint32_t>(file));
            anims_size;
            uint32_t anims_count = to_le(read<uint32_t>(file));
            skip(file, anims_count * sizeof(tr_animation));
        };

        // Endian flip is done during mesh generation.
        const auto read_meshdata = [&](auto& file)
        {
            skip(file, 4);
            uint32_t count = to_le(read<uint32_t>(file));
            _mesh_data = read_vector<uint16_t>(file, count);
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

        const std::unordered_map<std::string, std::function<void(std::basic_ispanstream<uint8_t>&)>> loader_functions
        {
            { "ANIMS", read_anims },
            { "CHANGES", read_generic },
            { "RANGES", read_generic },
            { "COMMANDS", read_generic },
            { "ANIBONES", read_list(_meshtree) },
            { "ANIMOBJ", read_animobj },
            { "STATOBJ", read_statobj },
            { "FRAMES", read_list(_frames) },
            { "MESHPTRS", read_list(_mesh_pointers) },
            { "MESHDATA", read_meshdata },
            { "OTEXTINF", read_list(object_textures) },
            { "OTEXTDAT", read_list(all_object_texture_data) },
            { "ITEXTINF", read_generic },
            { "ITEXTDAT", read_generic },
        };

        std::filesystem::path path{ _filename };
        path.replace_extension("SAD");
        const uint32_t sad_version = load_saturn_tagfile(*_files, path, loader_functions, "OBJEND");
        _platform_and_version.is_tr2_saturn = sad_version == 45;

        auto get_texture_for_cut = [&](auto&& offset, auto&& object_texture, bool is_cut) -> ObjectTextureData
            {
                const uint32_t start = offset << 3;
                const uint32_t end = start + (object_texture.size << 3);
                const std::vector<uint8_t> data = std::ranges::subrange(all_object_texture_data.begin() + start, all_object_texture_data.begin() + end)
                    | std::ranges::to<std::vector>();
                std::array<uint16_t, 16> palette;
                memcpy(&palette[0], &all_object_texture_data[end], sizeof(uint16_t) * 16);

                const uint32_t source_width = object_texture.width << 2;
                const uint32_t width = object_texture.width << 3;
                const uint32_t height = object_texture.height;

                std::vector<uint32_t> object_texture_data(width * height, 0);
                for (uint32_t y = 0; y < height; ++y)
                {
                    for (uint32_t x = 0; x < source_width; ++x)
                    {
                        const uint8_t value1 = ((data[y * source_width + x] & 0xF0) >> 4);
                        const uint8_t value2 = (data[y * source_width + x] & 0x0F);
                        object_texture_data[y * width + x * 2 + 0] = convert_saturn_palette(to_le(palette[value1]));
                        object_texture_data[y * width + x * 2 + 1] = convert_saturn_palette(to_le(palette[value2]));
                    }
                }

                return { .pixels = object_texture_data, .width = width, .height = height, .transparent_colour = convert_saturn_palette(to_le(palette[is_cut ? 15 : 0])) };
            };

        SaturnTextureInfo texture_info;
        TileMapper mapper(_num_textiles, [&](const std::vector<uint32_t>& data)
            {
                texture_info.textiles.push_back(data);
                ++_num_textiles;
            });

        // There are a lot of object textures that don't have any data - instead of creating a textile for each one of these
        // create a default blank texture and then redirect all entries to this.
        auto add_default_texture = [&]()
        {
            const uint16_t default_output_texture = static_cast<uint16_t>(_object_textures.size());
            _object_textures.push_back(mapper.map(std::vector<uint32_t>(256 * 256, 0), 256, 256));
            return default_output_texture;
        };

        const uint16_t default_output_texture = add_default_texture();
        uint16_t object_texture_index = 0;
        for (const auto& object_texture : object_textures)
        {
            if (trview::all_equal_to(1, object_texture.start, object_texture.cut0, object_texture.cut1, object_texture.cut2, object_texture.cut3))
            {
                texture_info.object_texture_mapping[object_texture_index][0] = { .index = default_output_texture };
                texture_info.object_texture_mapping[object_texture_index][1] = { .index = default_output_texture };
                texture_info.object_texture_mapping[object_texture_index][2] = { .index = default_output_texture };
                texture_info.object_texture_mapping[object_texture_index][3] = { .index = default_output_texture };
                texture_info.object_texture_mapping[object_texture_index][4] = { .index = default_output_texture };
            }
            else
            {
                const auto add_mapping = [&](const ObjectTextureData& texture, uint16_t operation)
                {
                    texture_info.object_texture_mapping[object_texture_index][operation] = { .index = static_cast<uint16_t>(_object_textures.size()), .transparent_colour = texture.transparent_colour };
                    _object_textures.push_back(mapper.map(texture.pixels, texture.width, texture.height));
                };

                if (object_texture.start != 1)
                {
                    add_mapping(get_texture_for_cut(object_texture.start, object_texture, false), 0);
                }
                else
                {
                    texture_info.object_texture_mapping[object_texture_index][0] = { .index = default_output_texture };
                }

                if (object_texture.cut0 != 1)
                {
                    add_mapping(get_texture_for_cut(object_texture.cut0, object_texture, true), 1);
                }

                if (object_texture.cut1 != 1)
                {
                    add_mapping(get_texture_for_cut(object_texture.cut1, object_texture, true), 2);
                }

                if (object_texture.cut2 != 1)
                {
                    add_mapping(get_texture_for_cut(object_texture.cut2, object_texture, true), 3);
                }

                if (object_texture.cut3 != 1)
                {
                    add_mapping(get_texture_for_cut(object_texture.cut3, object_texture, true), 4);
                }
            }

            ++object_texture_index;
        }

        mapper.finish();

        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
        callbacks.on_progress("Loading complete");

        std::unordered_set<uint16_t> transparent_object_textures;
        for (auto& [_, mesh] : _meshes)
        {
            for (auto& r : mesh.textured_rectangles)
            {
                const int16_t signed_tex = static_cast<int16_t>(r.texture);
                const int16_t tex = (signed_tex & 0x7fff) >> 4;
                const auto& mapping = texture_info.object_texture_mapping.find(tex);
                if (mapping != texture_info.object_texture_mapping.end())
                {
                    r.texture = (r.texture & 0x8000) | mapping->second[0].value().index;
                }
                if (r.effects != 0)
                {
                    transparent_object_textures.insert(r.texture & 0xFFF);
                }
            }

            for (auto& t : mesh.textured_triangles)
            {
                const uint16_t texture_operation = get_texture_operation(t.texture);
                const int16_t  tex = (static_cast<int16_t>(t.texture) & 0x1fff);

                const auto& mapping = texture_info.object_texture_mapping.find(tex);
                if (mapping != texture_info.object_texture_mapping.end())
                {
                    int16_t new_tex = mapping->second[texture_operation + 1].value_or(mapping->second[0].value()).index;
                    t.texture = (t.texture & 0xE000) | new_tex;
                }
                transparent_object_textures.insert(t.texture & 0xFFF);
            }
        }

        // Extract the mapped texture transparency values
        const auto texture_transparent_colours =
            texture_info.object_texture_mapping |
            std::views::values |
            std::views::join |
            std::views::filter([](auto&& map) { return map != std::nullopt && map.value().index != 0; }) |
            std::views::transform([](auto&& map) { return std::make_pair(map.value().index, map.value().transparent_colour); }) |
            std::ranges::to<std::unordered_map>();

        // Apply transparency.
        for (const auto& transparent_texture : transparent_object_textures)
        {
            const auto& found = texture_transparent_colours.find(transparent_texture);
            if (found != texture_transparent_colours.end())
            {
                const auto& object_texture = _object_textures[transparent_texture];
                const auto transparent_colour = found->second;
                auto& textile = texture_info.textiles[object_texture.TileAndFlag];
                for (uint32_t y = object_texture.Vertices[0].y_whole - 1; y < static_cast<uint32_t>(object_texture.Vertices[3].y_whole + 2); ++y)
                {
                    for (uint32_t x = object_texture.Vertices[0].x_whole - 1; x < static_cast<uint32_t>(object_texture.Vertices[3].x_whole + 2); ++x)
                    {
                        if (textile[y * 256 + x] == transparent_colour)
                        {
                            textile[y * 256 + x] = 0x00000000;
                        }
                    }
                }
            }
        }

        // Publish textures.
        for (const auto& info : texture_info.textiles)
        {
            callbacks.on_textile(info);
        }
    }

    void Level::load_tr1_saturn_snd(trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        activity;
        callbacks;

        const auto read_sampinfs = [&](auto& file)
        {
            skip(file, 4);
            uint32_t count = to_le(read<uint32_t>(file));
            _sound_details = 
                to_le(read_vector<tr_sound_details>(file, count)) | 
                std::views::transform([](auto&& s) -> tr_x_sound_details { return { .tr_sound_details = s }; }) |
                std::ranges::to<std::vector>();
        };

        const auto read_sample = [&](auto& file)
        {
            uint32_t index = to_le(read<uint32_t>(file));
            index;
            uint32_t count = to_le(read<uint32_t>(file));
            if (count != 16)
            {
                auto data = read_vector<uint8_t>(file, count);

                std::vector<uint8_t> results;
                results.resize(static_cast<uint32_t>(data.size() * 2));

                std::span out_span{ &results[0], results.size() };
                std::basic_ospanstream<uint8_t> out_stream{ out_span };
                out_stream.exceptions(std::istream::failbit | std::istream::badbit | std::istream::eofbit);

                out_stream << "RIFF";
                out_stream.seekp(4, std::ios::cur);
                out_stream << "WAVEfmt ";
                write(out_stream, 8);
                write<uint16_t>(out_stream, 1);
                write<uint16_t>(out_stream, 1);
                write<uint32_t>(out_stream, 11025);
                write<uint32_t>(out_stream, 11025);
                write<uint16_t>(out_stream, 1);
                write<uint16_t>(out_stream, 8);
                out_stream << "data";
                out_stream.seekp(4, std::ios::cur);

                // Convert signed PCM to unsigned.
                for (auto b : data)
                {
                    write<uint8_t>(out_stream, b + 127);
                }

                const uint32_t file_size = static_cast<uint32_t>(out_stream.tellp());
                out_stream.seekp(4, std::ios::beg);
                write<uint32_t>(out_stream, file_size - 8);
                out_stream.seekp(40, std::ios::beg);
                write<uint32_t>(out_stream, file_size - 44);
                results.resize(file_size);
                _sound_samples.push_back(results);
            }
            else
            {
                skip(file, 16);
                _sound_samples.push_back({});
            }
        };

        const std::unordered_map<std::string, std::function<void(std::basic_ispanstream<uint8_t>&)>> loader_functions
        {
            { "SAMPLUT", read_list(_sound_map) },
            { "SAMPINFS", read_sampinfs },
            { "SAMPLE", read_sample },
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
            { "SPRITDAT", read_list(spritdat) },
            { "SPRITINF", read_list(sprite_textures) },
            { "OBJECTS", read_objects },
        };

        std::filesystem::path path{ _filename };
        path.replace_extension("SPR");
        load_saturn_tagfile(*_files, path, loader_functions, "SPRITEND");

        TileMapper mapper(_num_textiles, [&](const std::vector<uint32_t>& data)
            {
                callbacks.on_textile(data);
                ++_num_textiles;
            });

        for (const auto sprite_texture : sprite_textures)
        {
            const auto data = 
                std::ranges::subrange(spritdat.begin() + (sprite_texture.start << 3), spritdat.begin() + (sprite_texture.end << 3))
              | std::ranges::to<std::vector>();

            const auto palette_bytes = 
                std::ranges::subrange(spritdat.begin() + (sprite_texture.end << 3), spritdat.begin() + (sprite_texture.end << 3) + 32)
              | std::ranges::to<std::vector>();

            std::array<uint16_t, 16> palette;
            memcpy(&palette[0], &palette_bytes[0], sizeof(uint16_t) * 16);

            uint32_t height = sprite_texture.height;
            uint32_t width = static_cast<uint32_t>(data.size() / height);

            std::vector<uint32_t> sprite_texture_data;
            sprite_texture_data.resize(height * width * 2);

            for (uint32_t y = 0; y < height; ++y)
            {
                for (uint32_t x = 0; x < width; ++x)
                {
                    uint8_t value1 = ((data[y * width + x] & 0xF0) >> 4);
                    uint8_t value2 = (data[y * width + x] & 0xF);

                    uint32_t pe1 = convert_saturn_palette(to_le(palette[value1]));
                    uint32_t pe2 = convert_saturn_palette(to_le(palette[value2]));

                    if (value1 == 0x0) { pe1 &= 0x00ffffff; }
                    if (value2 == 0x0) { pe2 &= 0x00ffffff; }

                    sprite_texture_data[y * (width * 2) + x * 2 + 0] = pe1;
                    sprite_texture_data[y * (width * 2) + x * 2 + 1] = pe2;
                }
            }

            _sprite_textures.push_back(mapper.map_sprite(sprite_texture_data, width * 2, height, sprite_texture));
        }

        mapper.finish();
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

        const uint16_t num_primitives = to_le(read<uint16_t>(stream));
        uint16_t total_primitives = 0;

        auto read_rects = [&](bool negate_texture = false) -> std::vector<tr_face4>
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
                    if (negate_texture)
                    {
                        r.texture |= 0x8000;
                    }
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
            const MeshPrimitive prim_type = to_le(read<MeshPrimitive>(stream));
            switch (prim_type)
            {
                case MeshPrimitive::ColouredTriangle:
                {
                    auto tris = read_tris();
                    mesh.coloured_triangles.append_range(tris);
                    break;
                }
                case MeshPrimitive::ColouredRectangle:
                {
                    auto rects = read_rects();
                    mesh.coloured_rectangles.append_range(rects);
                    break;
                }
                case MeshPrimitive::TexturedRectangle:
                case MeshPrimitive::TransparentRectangle:
                case MeshPrimitive::TexturedMirroredRectangle:
                case MeshPrimitive::TexturedMirroredTransparentRectangle:
                {
                    auto rects = convert_rectangles(read_rects(prim_type == MeshPrimitive::TexturedMirroredRectangle || prim_type == MeshPrimitive::TexturedMirroredTransparentRectangle));
                    if (prim_type == MeshPrimitive::TransparentRectangle || prim_type == MeshPrimitive::TexturedMirroredTransparentRectangle)
                    {
                        for (auto& rect : rects)
                        {
                            rect.effects = 1;
                        }
                    }
                    mesh.textured_rectangles.append_range(rects);
                    break;
                }
                case MeshPrimitive::UnknownTriangle:
                case MeshPrimitive::TransparentTriangle:
                case MeshPrimitive::TexturedTriangle:
                {
                    auto tris = convert_triangles(read_tris());
                    if (prim_type == MeshPrimitive::TransparentTriangle)
                    {
                        for (auto& tri : tris)
                        {
                            tri.effects = 1;
                        }
                    }
                    mesh.textured_triangles.append_range(tris);
                    break;
                }
            }
        }
    }
}