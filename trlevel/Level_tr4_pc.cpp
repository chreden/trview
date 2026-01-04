#include "Level.h"
#include "Level_common.h"

#include <ranges>
#include <span>

namespace trlevel
{
    namespace
    {
        struct NgleSoundSample
        {
            uint32_t start;
            uint32_t size;
        };

        std::vector<NgleSoundSample> read_sound_samples_ngle(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
        {
            uint32_t num_samples = read<uint32_t>(file);
            callbacks.on_progress("Reading NGLE sound samples");
            log_file(activity, file, std::format("Reading {} sound samples", num_samples));
            return read_vector<NgleSoundSample>(file, num_samples);
        }

        void read_room_lights_tr4_pc(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            log_file(activity, file, "Reading lights");
            room.lights = convert_lights(read_vector<uint16_t, tr4_room_light>(file));
            log_file(activity, file, std::format("Read {} lights", room.lights.size()));
        }

        void load_tr4_pc_room(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
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
            read_room_colour(activity, file, room);
            read_room_lights_tr4_pc(activity, file, room);
            read_room_static_meshes(activity, file, room);
            read_room_alternate_room(activity, file, room);
            read_room_flags(activity, file, room);
            read_room_water_scheme(activity, file, room);
            read_room_reverb_info(activity, file, room);
            read_room_alternate_group(activity, file, room);
        }

        uint32_t read_textiles_tr4_remastered(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
        {
            log_file(activity, file, "Reading textile counts");
            uint16_t num_room_textiles = read<uint16_t>(file);
            uint16_t num_obj_textiles = read<uint16_t>(file);
            uint16_t num_bump_textiles = read<uint16_t>(file);
            log_file(activity, file, std::format("Textile counts - Room:{}, Object:{}, Bump:{}", num_room_textiles, num_obj_textiles, num_bump_textiles));
            const auto num_textiles = num_room_textiles + num_obj_textiles + num_bump_textiles;

            callbacks.on_progress(std::format("Reading {} 32-bit textiles", num_textiles));
            log_file(activity, file, std::format("Reading {} 32-bit textiles", num_textiles));
            skip(file, 4); // skip sizes
            auto textile32 = read_vector<tr_textile32>(file, num_textiles);

            for (const auto& textile : textile32)
            {
                callbacks.on_textile(convert_textile(textile));
            }
            textile32 = {};

            log_file(activity, file, "Skipping misc textiles");
            const auto textile32_misc = read_vector<tr_textile32>(file, 2);
            for (const auto& textile : textile32_misc)
            {
                callbacks.on_textile(convert_textile(textile));
            }
            return num_textiles;
        }
    }

    bool is_ngle_sound_samples(trview::Activity&, std::basic_ispanstream<uint8_t>& file)
    {
        const auto position = file.tellg();
        uint32_t num_samples = read<uint32_t>(file);
        file.seekg(8 * num_samples, std::ios::cur);
        const auto ng = read<uint16_t>(file);
        file.seekg(position, std::ios::beg);
        return ng == 18254; // NG
    }

    void Level::load_tr4_pc(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        skip(file, 4); // version number
        _num_textiles = read_textiles_tr4_5(activity, file, callbacks);
        log_file(activity, file, "Reading and decompressing level data");
        callbacks.on_progress("Decompressing level data");
        const std::vector<uint8_t> level_data = read_compressed(file);
        std::basic_ispanstream<uint8_t> data_stream{ std::span(level_data) };
        callbacks.on_progress("Processing level data");

        {
            // Read unused value.
            read<uint32_t>(data_stream);

            if (data_stream.eof())
            {
                // VICT.TR2 ends here.
                return;
            }

            _rooms = read_rooms<uint16_t>(activity, data_stream, callbacks, load_tr4_pc_room);
            _floor_data = read_floor_data(activity, data_stream, callbacks);
            _mesh_data = read_mesh_data(activity, data_stream, callbacks);
            _mesh_pointers = read_mesh_pointers(activity, data_stream, callbacks);
            _animations = read_animations_tr4_5(activity, data_stream, callbacks);
            _state_changes = read_state_changes(activity, data_stream, callbacks);
            _anim_dispatches = read_anim_dispatches(activity, data_stream, callbacks);
            read_anim_commands(activity, data_stream, callbacks);
            _meshtree = read_meshtree(activity, data_stream, callbacks);
            _frames = read_frames(activity, data_stream, callbacks);
            _models = read_models_tr1_4(activity, data_stream, callbacks);
            _static_meshes = read_static_meshes(activity, data_stream, callbacks);
            log_file(activity, data_stream, "Skipping SPR marker");
            // Skip past the 'SPR' marker.
            data_stream.seekg(3, std::ios::cur);
            _sprite_textures = read_sprite_textures(activity, data_stream, callbacks);
            _sprite_sequences = read_sprite_sequences(activity, data_stream, callbacks);
            _cameras = read_cameras(activity, data_stream, callbacks);
            _flyby_cameras = read_flyby_cameras(activity, data_stream, callbacks);

            _sound_sources = read_sound_sources(activity, data_stream, callbacks);
            const auto boxes = read_boxes(activity, data_stream, callbacks);
            read_overlaps(activity, data_stream, callbacks);
            read_zones(activity, data_stream, callbacks, static_cast<uint32_t>(boxes.size()));
            _animated_textures = read_animated_textures(activity, data_stream, callbacks);
            _animated_texture_uv_count = read_animated_textures_uv_count(activity, data_stream, callbacks);

            log_file(activity, data_stream, "Skipping TEX marker");
            data_stream.seekg(3, std::ios::cur);

            _object_textures = read_object_textures_tr4_5(activity, data_stream, callbacks);
            _entities = read_entities(activity, data_stream, callbacks);
            _ai_objects = read_ai_objects(activity, data_stream, callbacks);
            read_demo_data(activity, data_stream, callbacks);
            _sound_map = read_sound_map(activity, data_stream, callbacks);
            _sound_details = read_sound_details(activity, data_stream, callbacks);
            _sample_indices = read_sample_indices(activity, data_stream, callbacks);
        }

        const auto sound_start = file.tellg();
        try
        {
            _trng = is_ngle_sound_samples(activity, file);
            if (_trng)
            {
                load_ngle_sound_fx(activity, file, callbacks);
            }
            else
            {
                read_sound_samples_tr4_5(file, activity, callbacks);
            }
        }
        catch (const std::exception& e)
        {
            callbacks.on_progress("Failed to load sound samples");
            file.clear();
            file.seekg(sound_start, std::ios::beg);
            log_file(activity, file, std::format("Failed to load sound samples {}", e.what()));
        }

        // Final check for NG-ness
        const auto ng_start = file.tellg();
        try
        {
            const auto ng = read<uint16_t>(file);
            file.seekg(ng_start, std::ios::beg);
            _trng |= ng == 18254;
        }
        catch (const std::exception&)
        {
            file.clear();
            file.seekg(sound_start, std::ios::beg);
        }

        if (_trng)
        {
            log_file(activity, file, "TRNG level detected");
        }

        generate_sounds(callbacks);
        callbacks.on_progress("Generating meshes");
        log_file(activity, file, "Generating meshes");
        generate_meshes(_mesh_data);
    }

    void Level::load_tr4_pc_remastered(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        skip(file, 4); // version number
        _num_textiles = read_textiles_tr4_remastered(activity, file, callbacks);
        log_file(activity, file, "Reading level data");
        callbacks.on_progress("Processing level data");

        _rooms = read_rooms<uint16_t>(activity, file, callbacks, load_tr4_pc_room);
        _floor_data = read_floor_data(activity, file, callbacks);
        _mesh_data = read_mesh_data(activity, file, callbacks);
        _mesh_pointers = read_mesh_pointers(activity, file, callbacks);
        _animations = read_animations_tr4_5(activity, file, callbacks);
        _state_changes = read_state_changes(activity, file, callbacks);
        _anim_dispatches = read_anim_dispatches(activity, file, callbacks);
        read_anim_commands(activity, file, callbacks);
        _meshtree = read_meshtree(activity, file, callbacks);
        _frames = read_frames(activity, file, callbacks);
        _models = read_models_tr1_4(activity, file, callbacks);
        _static_meshes = read_static_meshes(activity, file, callbacks);
        log_file(activity, file, "Skipping SPR marker");
        // Skip past the 'SPR' marker.
        file.seekg(3, std::ios::cur);
        _sprite_textures = read_sprite_textures(activity, file, callbacks);
        _sprite_sequences = read_sprite_sequences(activity, file, callbacks);
        _cameras = read_cameras(activity, file, callbacks);
        read_flyby_cameras(activity, file, callbacks);

        _sound_sources = read_sound_sources(activity, file, callbacks);
        const auto boxes = read_boxes(activity, file, callbacks);
        read_overlaps(activity, file, callbacks);
        read_zones(activity, file, callbacks, static_cast<uint32_t>(boxes.size()));
        _animated_textures = read_animated_textures(activity, file, callbacks);
        _animated_texture_uv_count = read_animated_textures_uv_count(activity, file, callbacks);

        log_file(activity, file, "Skipping TEX marker");
        file.seekg(3, std::ios::cur);

        _object_textures = read_object_textures_tr4_5(activity, file, callbacks);
        _entities = read_entities(activity, file, callbacks);
        _ai_objects = read_ai_objects(activity, file, callbacks);
        load_sound_fx(activity, callbacks);
        generate_sounds(callbacks);

        callbacks.on_progress("Generating meshes");
        log_file(activity, file, "Generating meshes");
        generate_meshes(_mesh_data);
    }

    void Level::read_sound_samples_tr4_5(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const ILevel::LoadCallbacks& callbacks)
    {
        uint32_t num_samples = read<uint32_t>(file);
        callbacks.on_progress("Reading sound samples");
        log_file(activity, file, std::format("Reading {} sound samples", num_samples));
        for (uint32_t i = 0; i < num_samples; ++i)
        {
            uint32_t uncompressed = read<uint32_t>(file);
            uncompressed;
            uint32_t compressed = read<uint32_t>(file);
            _sound_samples.push_back(read_vector<uint8_t>(file, compressed));
        }
        log_file(activity, file, std::format("Read {} sound samples", num_samples));
    }

    void Level::load_ngle_sound_fx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const LoadCallbacks& callbacks)
    {
        const auto ngle_samples = read_sound_samples_ngle(activity, file, callbacks);
        if (const auto main = load_main_sfx())
        {
            std::basic_ispanstream<uint8_t> sfx_file{ std::span(*main) };
            sfx_file.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);

            for (uint32_t i = 0; i < ngle_samples.size(); ++i)
            {
                const auto sample = ngle_samples[i];
                sfx_file.seekg(sample.start);
                _sound_samples.push_back(read_vector<uint8_t>(sfx_file, sample.size));
            }
        }
    }
}