#include "Level.h"
#include "Level_common.h"
#include "Level_tr3.h"

#include <ranges>

namespace trlevel
{
    namespace
    {
        void load_tr3_pc_room(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            room.info = read_room_info(activity, file);
            uint32_t NumDataWords = read_num_data_words(activity, file);
            // Read actual room data.
            if (NumDataWords > 0)
            {
                read_room_vertices_tr3_4(activity, file, room);
                read_room_rectangles(activity, file, room);
                read_room_triangles(activity, file, room);
                read_room_sprites(activity, file, room);
            }

            read_room_portals(activity, file, room);
            read_room_sectors(activity, file, room);
            read_room_ambient_intensity_1(activity, file, room);
            read_room_light_mode(activity, file, room);
            read_room_lights_tr3(activity, file, room);
            read_room_static_meshes(activity, file, room);
            read_room_alternate_room(activity, file, room);
            read_room_flags(activity, file, room);
            read_room_water_scheme(activity, file, room);
            read_room_reverb_info(activity, file, room);
            skip(file, 1); // filler in TR3
        }
    }

    void Level::load_tr3_pc(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        skip(file, 4); // version number
        read_palette_tr2_3(file, activity, callbacks);
        _num_textiles = read_textiles(activity, file, callbacks);

        // Read unused value.
        read<uint32_t>(file);

        if (file.eof())
        {
            // VICT.TR2 ends here.
            return;
        }

        _rooms = read_rooms<uint16_t>(activity, file, callbacks, load_tr3_pc_room);
        _floor_data = read_floor_data(activity, file, callbacks);
        _mesh_data = read_mesh_data(activity, file, callbacks);
        _mesh_pointers = read_mesh_pointers(activity, file, callbacks);
        _animations = convert_animations(read_animations_tr1_3(activity, file, callbacks));
        _state_changes = read_state_changes(activity, file, callbacks);
        _anim_dispatches = read_anim_dispatches(activity, file, callbacks);
        read_anim_commands(activity, file, callbacks);
        _meshtree = read_meshtree(activity, file, callbacks);
        _frames = read_frames(activity, file, callbacks);
        _models = read_models_tr1_4(activity, file, callbacks);
        _static_meshes = read_static_meshes(activity, file, callbacks);
        _sprite_textures = read_sprite_textures(activity, file, callbacks);
        _sprite_sequences = read_sprite_sequences(activity, file, callbacks);
        _cameras = read_cameras(activity, file, callbacks);
        _sound_sources = read_sound_sources(activity, file, callbacks);
        const auto boxes = read_boxes(activity, file, callbacks);
        read_overlaps(activity, file, callbacks);
        read_zones(activity, file, callbacks, static_cast<uint32_t>(boxes.size()));
        _animated_textures = read_animated_textures(activity, file, callbacks);
        _object_textures = read_object_textures_tr1_3(activity, file, callbacks);
        _entities = read_entities(activity, file, callbacks);
        read_light_map(activity, file, callbacks);
        read_cinematic_frames(activity, file, callbacks);
        read_demo_data(activity, file, callbacks);
        _sound_map = read_sound_map(activity, file, callbacks);
        _sound_details = read_sound_details(activity, file, callbacks);
        _sample_indices = read_sample_indices(activity, file, callbacks);
        load_sound_fx(activity, callbacks);
        generate_sounds(callbacks);
        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
        callbacks.on_progress("Loading complete");
    }

    void read_room_lights_tr3(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading lights");
        room.lights = convert_lights(read_vector<uint16_t, tr3_room_light>(file));
        log_file(activity, file, std::format("Read {} lights", room.lights.size()));
    }
}
