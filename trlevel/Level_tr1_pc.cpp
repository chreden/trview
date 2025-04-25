#include "Level.h"
#include "Level_common.h"
#include "Level_tr1.h"

#include <ranges>

namespace trlevel
{
    namespace
    {
        void read_room_lights_tr1_pc(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            log_file(activity, file, "Reading lights");
            room.lights = convert_lights(read_vector<uint16_t, tr_room_light>(file));
            log_file(activity, file, std::format("Read {} lights", room.lights.size()));
        }

        void read_room_static_meshes_tr1_pc(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            log_file(activity, file, "Reading static meshes");
            room.static_meshes = convert_room_static_meshes(read_vector<uint16_t, tr_room_staticmesh>(file));
            log_file(activity, file, std::format("Read {} static meshes", room.static_meshes.size()));
        }

        void load_tr1_pc_room(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            room.info = read_room_info(activity, file);
            uint32_t NumDataWords = read_num_data_words(activity, file);

            // Read actual room data.
            if (NumDataWords > 0)
            {
                read_room_vertices_tr1(activity, file, room);
                read_room_rectangles(activity, file, room);
                read_room_triangles(activity, file, room);
                read_room_sprites(activity, file, room);
            }

            read_room_portals(activity, file, room);
            read_room_sectors(activity, file, room);
            read_room_ambient_intensity_1(activity, file, room);
            read_room_lights_tr1_pc(activity, file, room);
            read_room_static_meshes_tr1_pc(activity, file, room);
            read_room_alternate_room(activity, file, room);
            read_room_flags(activity, file, room);
        }
    }

    std::vector<tr_box> read_boxes_tr1(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading boxes");
        log_file(activity, file, "Reading boxes");
        const auto boxes = read_vector<uint32_t, tr_box>(file);
        log_file(activity, file, std::format("Read {} boxes", static_cast<uint32_t>(boxes.size())));
        return boxes;
    }

    std::vector<tr2_entity> read_entities_tr1(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading entities");
        log_file(activity, file, "Reading entities");
        auto entities = convert_entities(read_vector<uint32_t, tr_entity>(file));
        log_file(activity, file, std::format("Read {} entities", entities.size()));
        return entities;
    }

    void read_room_vertices_tr1(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading vertices");
        room.data.vertices = convert_vertices(read_vector<int16_t, tr_room_vertex>(file));
        log_file(activity, file, std::format("Read {} vertices", room.data.vertices.size()));
    }

    void read_zones_tr1(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks, uint32_t num_boxes)
    {
        callbacks.on_progress("Reading zones");
        log_file(activity, file, "Reading zones");
        std::vector<int16_t> zones = read_vector<int16_t>(file, num_boxes * 6);
        log_file(activity, file, std::format("Read {} zones", zones.size()));
    }

    void Level::generate_sounds_tr1(const LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Generating sounds");
        for (auto i = 0; i < _sample_indices.size(); ++i)
        {
            const auto start = _sample_indices[i];
            const auto end = i + 1 < _sample_indices.size() ? _sample_indices[i + 1] : _sound_data.size();
            callbacks.on_sound(static_cast<int16_t>(i), { _sound_data.begin() + start, _sound_data.begin() + end });
        }
    }

    void Level::load_tr1_pc(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        read_textiles_tr1_pc(file, activity, callbacks);
        read<uint32_t>(file);

        _rooms = read_rooms<uint16_t>(activity, file, callbacks, load_tr1_pc_room);
        _floor_data = read_floor_data(activity, file, callbacks);
        _mesh_data = read_mesh_data(activity, file, callbacks);
        _mesh_pointers = read_mesh_pointers(activity, file, callbacks);
        read_animations_tr1_3(activity, file, callbacks);
        read_state_changes(activity, file, callbacks);
        read_anim_dispatches(activity, file, callbacks);
        read_anim_commands(activity, file, callbacks);
        _meshtree = read_meshtree(activity, file, callbacks);
        _frames = read_frames(activity, file, callbacks);
        _models = read_models_tr1_4(activity, file, callbacks);
        _static_meshes = read_static_meshes(activity, file, callbacks);
        _object_textures = read_object_textures_tr1_3(activity, file, callbacks);
        _sprite_textures = read_sprite_textures(activity, file, callbacks);
        _sprite_sequences = read_sprite_sequences(activity, file, callbacks);

        // Special handling for demo levels or levels where the palette is here:
        bool on_demo_attempt = false;
        const auto branch_start = file.tellg();
        std::optional<std::exception> original_exception;

        do
        {
            try
            {
                if (on_demo_attempt)
                {
                    read_palette_tr1(file, activity, callbacks);
                }

                _cameras = read_cameras(activity, file, callbacks);
                _sound_sources = read_sound_sources(activity, file, callbacks);
                const auto boxes = read_boxes_tr1(activity, file, callbacks);
                read_overlaps(activity, file, callbacks);
                read_zones_tr1(activity, file, callbacks, static_cast<uint32_t>(boxes.size()));
                read_animated_textures(activity, file, callbacks);
                _entities = read_entities_tr1(activity, file, callbacks);
                read_light_map(activity, file, callbacks);

                if (!on_demo_attempt)
                {
                    read_palette_tr1(file, activity, callbacks);
                }

                for (const auto& t : _textile8)
                {
                    callbacks.on_textile(t.Tile |
                        std::views::transform([&](uint8_t entry_index)
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
                            }) | std::ranges::to<std::vector>());
                }
                _textile8 = {};

                read_cinematic_frames(activity, file, callbacks);
                read_demo_data(activity, file, callbacks);
                _sound_map = read_sound_map(activity, file, callbacks);
                _sound_details = read_sound_details(activity, file, callbacks);
                _sound_data = read_sound_data(activity, file, callbacks);
                _sample_indices = read_sample_indices(activity, file, callbacks);
                break;
            }
            catch (const std::exception& e)
            {
                if (!on_demo_attempt)
                {
                    original_exception = e;
                    callbacks.on_progress("Attempting to load as TR1 demo");
                    file.clear();
                    file.seekg(branch_start, std::ios::beg);
                    on_demo_attempt = true;
                }
                else
                {
                    throw original_exception.value_or(e);
                }
            }

        } while (on_demo_attempt);

        generate_sounds_tr1(callbacks);
        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
        callbacks.on_progress("Loading complete");
    }

    void Level::read_palette_tr1(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading 8-bit palette");
        log_file(activity, file, "Reading 8-bit palette");
        _palette = read_vector<tr_colour>(file, 256);
        log_file(activity, file, "Read 8-bit palette");
    }

    void Level::read_textiles_tr1_pc(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading textiles");
        log_file(activity, file, "Reading textiles");
        _num_textiles = read<uint32_t>(file);
        callbacks.on_progress(std::format("Reading {} 8-bit textiles", _num_textiles));
        log_file(activity, file, std::format("Reading {} 8-bit textiles", _num_textiles));
        _textile8 = read_vector<tr_textile8>(file, _num_textiles);
    }
}
