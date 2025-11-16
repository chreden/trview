#include "Level.h"
#include "Level_common.h"
#include "Level_psx.h"
#include <trview.common/Algorithms.h>

#include <ranges>

namespace trlevel
{
    namespace
    {
        tr_x_room_light to_tr5_light(const tr4_psx_room_light& l)
        {
            tr_x_room_light new_light
            {
                .level_version = LevelVersion::Tomb5,
                .tr5 =
                {
                    .position = {.x = static_cast<float>(l.x), .y = static_cast<float>(l.y), .z = static_cast<float>(l.z) },
                    .colour = {.r = static_cast<float>(l.r) / 255.0f, .g = static_cast<float>(l.g) / 255.0f, .b = static_cast<float>(l.b) / 255.0f },
                    .direction = {.x = static_cast<float>(l.dx) / 4096.0f, .y = static_cast<float>(l.dy) / 4096.0f, .z = static_cast<float>(l.dz) / 4096.0f },
                    .light_type = l.type
                }
            };

            switch (l.type)
            {
            case LightType::Spot:
            {
                new_light.tr5.in = static_cast<float>(l.spot.in << 2) / 16384.0f;
                new_light.tr5.out = static_cast<float>(l.spot.out << 2) / 16384.0f;
                new_light.tr5.rad_in = std::acosf(new_light.tr5.in) * 2.0f;
                new_light.tr5.rad_out = std::acosf(new_light.tr5.out) * 2.0f;
                new_light.tr5.range = static_cast<float>(l.spot.cutoff << 7);
                break;
            }
            case LightType::Shadow:
            {
                new_light.tr5.in = static_cast<float>(l.shadow.hotspot << 7);
                new_light.tr5.out = static_cast<float>(l.shadow.falloff << 7);
                break;
            }
            case LightType::Point:
            {
                new_light.tr5.in = static_cast<float>(l.point.hotspot << 7);
                new_light.tr5.out = static_cast<float>(l.point.falloff << 7);
                break;
            }
            }

            return new_light;
        }

        std::vector<tr3_room> read_rooms_tr5_psx(uint16_t num_rooms, std::basic_ispanstream<uint8_t>& file)
        {
            return read_vector<tr4_psx_room_info>(file, num_rooms)
                | std::views::transform([&](auto&& room_info)
                    {
                        tr3_room room
                        {
                            .info = room_info.info,
                            .num_z_sectors = room_info.num_z_sectors,
                            .num_x_sectors = room_info.num_x_sectors,
                            .alternate_room = room_info.alternate_room,
                            .flags = room_info.flags,
                            .alternate_group = room_info.alternate_group,
                        };

                        read_room_geometry_tr4_psx(file, room, room_info.data_size);

                        const uint32_t portals_start = static_cast<uint32_t>(file.tellg());
                        room.portals = read_vector<tr_room_portal>(file, room_info.portal_size / sizeof(tr_room_portal));
                        file.seekg(portals_start + room_info.portal_size, std::ios::beg);

                        const uint32_t sectors_start = static_cast<uint32_t>(file.tellg());
                        room.sector_list = read_vector<tr_room_sector>(file, room.num_z_sectors * room.num_x_sectors);
                        file.seekg(sectors_start + room_info.sectors_size, std::ios::beg);

                        const uint32_t lights_start = static_cast<uint32_t>(file.tellg());
                        room.lights = read_vector<tr4_psx_room_light>(file, room_info.num_lights)
                            | std::views::transform(to_tr5_light)
                            | std::ranges::to<std::vector>();
                        file.seekg(lights_start + room_info.light_size, std::ios::beg);

                        const uint32_t statics_start = static_cast<uint32_t>(file.tellg());
                        room.static_meshes = read_vector<tr3_room_staticmesh>(file, room_info.num_meshes);
                        file.seekg(statics_start + room_info.static_mesh_size, std::ios::beg);

                        return room;
                    }) | std::ranges::to<std::vector>();
        }

        constexpr uint32_t model_count(PlatformAndVersion version)
        {
            switch (version.raw_version)
            {
            case -201:
                return 305;
            case -202:
                return 312;
            case -206:
                return 378;
            case -209:
                return 393;
            case -214:
                return 427;
            case -215:
                return 431;
            case -219:
                return 441;
            case -223:
            case -224:
                return 454;
            }
            return 460;
        }

        constexpr uint32_t static_count(PlatformAndVersion version)
        {
            switch (version.raw_version)
            {
            case -201:
            case -202:
                return 60;
            }
            return 70;
        }
    }

    void Level::read_sound_map_tr5_psx(std::basic_ispanstream<uint8_t>& file, const tr4_psx_level_info& info, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        // October version has a smaller sound map. Attempt to load with retail sound map and revert to
        // smaller sound map if the entities don't look good (too many Laras).
        const auto before_sound_map = file.tellg();
        for (const auto sound_map_size : { 450, 370 })
        {
            file.seekg(before_sound_map);
            _sound_map = read_vector<int16_t>(file, sound_map_size);
            _sound_details = read_vector<tr_x_sound_details>(file, info.sample_info_length / sizeof(tr_x_sound_details));
            const auto before_entities = file.tellg();
            const auto entities = read_entities(activity, file, info, callbacks);
            const auto lara_count = std::ranges::count_if(entities, [](auto&& e) { return e.TypeID == 0; });
            if (lara_count < 2)
            {
                file.seekg(before_entities);
                return;
            }
        }
    }

    void Level::load_tr5_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        if (!is_supported_tr5_psx_version(peek<int32_t>(file)))
        {
            file.seekg(313344);
        }
        const uint32_t start = static_cast<uint32_t>(file.tellg());
        auto info = read<tr4_psx_level_info>(file);
        file.seekg(start + info.room_data_offset, std::ios::beg);
        _rooms = read_rooms_tr5_psx(info.num_rooms, file);
        _floor_data = read_vector<uint16_t>(file, info.floor_data_size / 2);

        // 'Room outside map':
        read_vector<uint16_t>(file, 729);
        skip(file, 2);
        // Outside room table:
        read_vector<uint8_t>(file, info.outside_room_size);
        // Bounding boxes
        skip(file, info.bounding_boxes_size);

        _mesh_data = read_mesh_data(activity, file, info, callbacks);
        _mesh_pointers = read_mesh_pointers(activity, file, info, callbacks);
        skip(file, info.animations_size); // TODO: Load animations
        skip(file, info.state_changes_size);
        skip(file, info.dispatches_size);
        skip(file, info.commands_size);
        _meshtree = read_meshtree(activity, file, info, callbacks);
        _animated_textures = read_animated_textures_tr4_psx(activity, file, info, callbacks);
        _animated_texture_uv_count = info.num_animated_uv_ranges;
        _object_textures_psx = read_object_textures(activity, file, info, callbacks);
        skip(file, info.sprite_info_length);
        adjust_room_textures_psx();
        _object_textures_psx.append_range(read_room_textures(activity, file, info, callbacks));
        _sound_sources = read_sound_sources(activity, file, info, callbacks);
        read_sound_map_tr5_psx(file, info, activity, callbacks);
        _entities = read_entities(activity, file, info, callbacks);
        _ai_objects = read_ai_objects(activity, file, info, callbacks);
        skip(file, info.boxes_length + info.overlaps_length);
        skip(file, 2 * (info.ground_zone_length + info.ground_zone_length2 + info.ground_zone_length3 + info.ground_zone_length4 + info.ground_zone_length5));
        _cameras = read_vector<tr_camera>(file, info.num_cameras);
        _flyby_cameras = read_vector<tr4_flyby_camera>(file, info.num_flyby_cameras);
        _frames = read_frames(activity, file, start, info, callbacks);
        _models = read_models(activity, file, start, info, callbacks, model_count(_platform_and_version));
        _static_meshes = read_static_meshes_tr4_psx(activity, file, callbacks, static_count(_platform_and_version));
        generate_object_textures_tr4_psx(file, start, info);

        for (const auto& t : _textile16)
        {
            callbacks.on_textile(convert_textile(t));
        }

        read_sounds_tr4_psx(file, activity, callbacks, start, info, 11025);
        generate_sounds(callbacks);

        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
        callbacks.on_progress("Loading complete");
    }
}