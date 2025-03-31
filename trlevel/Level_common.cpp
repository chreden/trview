#include "Level_common.h"

namespace trlevel
{
    namespace
    {
        void skip_vector_compressed(std::basic_ispanstream<uint8_t>& file)
        {
            skip(file, sizeof(uint32_t));
            auto compressed_size = read<uint32_t>(file);
            skip(file, compressed_size);
        }
    }

    void skip(std::basic_ispanstream<uint8_t>& file, uint32_t size)
    {
        file.seekg(size, std::ios::cur);
    }

    void log_file(trview::Activity& activity, std::basic_ispanstream<uint8_t>& stream, const std::string& text)
    {
        activity.log(std::format("[{}] {}", static_cast<uint64_t>(stream.tellg()), text));
    }

    void log_file(trview::Activity& activity, std::istream& stream, const std::string& text)
    {
        activity.log(std::format("[{}] {}", static_cast<uint64_t>(stream.tellg()), text));
    }

    std::vector<uint8_t> read_compressed(std::basic_ispanstream<uint8_t>& file)
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

    std::vector<tr4_ai_object> read_ai_objects(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading AI objects");
        log_file(activity, file, "Reading AI objects");
        const auto ai_objects = read_vector<uint32_t, tr4_ai_object>(file);
        log_file(activity, file, std::format("Read {} AI objects", ai_objects.size()));
        return ai_objects;
    }

    std::vector<uint16_t> read_animated_textures(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading animated textures");
        log_file(activity, file, "Reading animated textures");
        std::vector<uint16_t> animated_textures = read_vector<uint32_t, uint16_t>(file);
        log_file(activity, file, std::format("Read {} animated textures", animated_textures.size()));
        return animated_textures;
    }

    uint8_t read_animated_textures_uv_count(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading animated textures UV count");
        log_file(activity, file, "Reading animated textures UV count");
        uint8_t animated_textures_uv_count = read<uint8_t>(file);
        log_file(activity, file, std::format("Animated texture UV count: {}", animated_textures_uv_count));
        return animated_textures_uv_count;
    }

    std::vector<tr_animation> read_animations_tr1_3(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading animations");
        log_file(activity, file, "Reading animations");
        return read_vector<uint32_t, tr_animation>(file);
    }

    std::vector<tr4_animation> read_animations_tr4_5(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading animations");
        log_file(activity, file, "Reading animations");
        return read_vector<uint32_t, tr4_animation>(file);
    }

    std::vector<tr_anim_command> read_anim_commands(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading anim commands");
        log_file(activity, file, "Reading anim commands");
        const auto anim_commands = read_vector<uint32_t, tr_anim_command>(file);
        log_file(activity, file, std::format("Read {} anim commands", anim_commands.size()));
        return anim_commands;
    }

    std::vector<tr_anim_dispatch> read_anim_dispatches(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading anim dispatches");
        log_file(activity, file, "Reading anim dispatches");
        const auto anim_dispatches = read_vector<uint32_t, tr_anim_dispatch>(file);
        log_file(activity, file, std::format("Read {} anim dispatches", anim_dispatches.size()));
        return anim_dispatches;
    }

    std::vector<tr2_box> read_boxes(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading boxes");
        log_file(activity, file, "Reading boxes");
        std::vector<tr2_box> boxes = read_vector<uint32_t, tr2_box>(file);
        uint32_t num_boxes = static_cast<uint32_t>(boxes.size());
        log_file(activity, file, std::format("Read {} boxes", num_boxes));
        return boxes;
    }

    std::vector<tr_camera> read_cameras(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading cameras");
        log_file(activity, file, "Reading cameras");
        const auto cameras = read_vector<uint32_t, tr_camera>(file);
        log_file(activity, file, std::format("Read {} cameras", cameras.size()));
        return cameras;
    }

    std::vector<tr_cinematic_frame> read_cinematic_frames(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading cinematic frames");
        log_file(activity, file, "Reading cinematic frames");
        std::vector<tr_cinematic_frame> cinematic_frames = read_vector<uint16_t, tr_cinematic_frame>(file);
        log_file(activity, file, std::format("Read {} cinematic frames", cinematic_frames.size()));
        return cinematic_frames;
    }

    std::vector<uint8_t> read_demo_data(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading demo data");
        log_file(activity, file, "Reading demo data");
        auto demo_data = read_vector<uint16_t, uint8_t>(file);
        log_file(activity, file, std::format("Read {} demo data", demo_data.size()));
        return demo_data;
    }

    std::vector<tr2_entity> read_entities(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading entities");
        log_file(activity, file, "Reading entities");
        // TR4 entity is in here, OCB is not set but goes into intensity2 (convert later).
        const auto entities = read_vector<uint32_t, tr2_entity>(file);
        log_file(activity, file, std::format("Read {} entities", entities.size()));
        return entities;
    }

    std::vector<uint16_t> read_floor_data(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading floor data");
        log_file(activity, file, "Reading floor data");
        const auto floor_data = read_vector<uint32_t, uint16_t>(file);
        log_file(activity, file, std::format("Read {} floor data", floor_data.size()));
        return floor_data;
    }

    std::vector<tr4_flyby_camera> read_flyby_cameras(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading flyby cameras");
        log_file(activity, file, "Reading flyby cameras");
        std::vector<tr4_flyby_camera> flyby_cameras = read_vector<uint32_t, tr4_flyby_camera>(file);
        log_file(activity, file, std::format("Read {} flyby cameras", flyby_cameras.size()));
        return flyby_cameras;
    }

    std::vector<uint16_t> read_frames(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading frames");
        log_file(activity, file, "Reading frames");
        const auto frames = read_vector<uint32_t, uint16_t>(file);
        log_file(activity, file, std::format("Read {} frames", frames.size()));
        return frames;
    }

    std::vector<uint8_t> read_light_map(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading light map");
        log_file(activity, file, "Reading light map");
        std::vector<uint8_t> light_map = read_vector<uint8_t>(file, 32 * 256);
        log_file(activity, file, "Read light map");
        return light_map;
    }

    std::vector<uint16_t> read_mesh_data(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading mesh data");
        log_file(activity, file, "Reading mesh data");
        const auto mesh_data = read_vector<uint32_t, uint16_t>(file);
        log_file(activity, file, std::format("Read {} mesh data", mesh_data.size()));
        return mesh_data;
    }

    std::vector<uint32_t> read_mesh_pointers(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading mesh pointers");
        log_file(activity, file, "Reading mesh pointers");
        const auto mesh_pointers = read_vector<uint32_t, uint32_t>(file);
        log_file(activity, file, std::format("Read {} mesh pointers", mesh_pointers.size()));
        return mesh_pointers;
    }

    std::vector<uint32_t> read_meshtree(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading mesh trees");
        log_file(activity, file, "Reading mesh trees");
        const auto meshtree = read_vector<uint32_t, uint32_t>(file);
        log_file(activity, file, std::format("Read {} mesh trees", meshtree.size()));
        return meshtree;
    }

    std::vector<tr_model> read_models_tr1_4(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading models");
        log_file(activity, file, "Reading models");
        auto models = read_vector<uint32_t, tr_model>(file);
        log_file(activity, file, std::format("Read {} models", models.size()));
        return models;
    }

    uint32_t read_num_data_words(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file)
    {
        log_file(activity, file, "Reading number of data words");
        uint32_t NumDataWords = read<uint32_t>(file);
        log_file(activity, file, std::format("{} data words to process", NumDataWords));
        return NumDataWords;
    }

    std::vector<tr_object_texture> read_object_textures_tr1_3(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading object textures");
        log_file(activity, file, "Reading object textures");
        auto object_textures = read_vector<uint32_t, tr_object_texture>(file);
        log_file(activity, file, std::format("Read {} object textures", object_textures.size()));
        return object_textures;
    }

    std::vector<tr_object_texture> read_object_textures_tr4_5(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading object textures");
        log_file(activity, file, "Reading object textures");
        const auto object_textures = convert_object_textures(read_vector<uint32_t, tr4_object_texture>(file));
        log_file(activity, file, std::format("Read {} object textures", object_textures.size()));
        return object_textures;
    }

    std::vector<uint16_t> read_overlaps(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading overlaps");
        log_file(activity, file, "Reading overlaps");
        std::vector<uint16_t> overlaps = read_vector<uint32_t, uint16_t>(file);
        log_file(activity, file, std::format("Read {} overlaps", overlaps.size()));
        return overlaps;
    }

    void read_room_alternate_group(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading alternate group");
        room.alternate_group = read<uint8_t>(file);
        log_file(activity, file, std::format("Read alternate group: {}", room.alternate_group));
    }

    void read_room_alternate_room(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading alternate room");
        room.alternate_room = read<int16_t>(file);
        log_file(activity, file, std::format("Read alternate room: {}", room.alternate_room));
    }

    void read_room_ambient_intensity_1(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading ambient intensity 1");
        room.ambient_intensity_1 = read<int16_t>(file);
        log_file(activity, file, std::format("Read ambient intensity 1: {}", room.ambient_intensity_1));
    }

    void read_room_colour(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading room colour");
        room.colour = read<uint32_t>(file);
        log_file(activity, file, std::format("Read room colour {:X}", room.colour));
    }

    void read_room_flags(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading flags");
        room.flags = read<int16_t>(file);
        log_file(activity, file, std::format("Read flags: {:X}", room.flags));
    }

    tr_room_info read_room_info(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file)
    {
        log_file(activity, file, "Reading room info");
        auto info = convert_room_info(read<tr1_4_room_info>(file));
        log_file(activity, file, "Read room info");
        return info;
    }

    void read_room_light_mode(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading light mode");
        room.light_mode = read<int16_t>(file);
        log_file(activity, file, std::format("Read light mode: {}", room.light_mode));
    }

    void read_room_portals(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading portals");
        room.portals = read_vector<uint16_t, tr_room_portal>(file);
        log_file(activity, file, std::format("Read {} portals", room.portals.size()));
    }

    void read_room_rectangles(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading rectangles");
        room.data.rectangles = convert_rectangles(read_vector<int16_t, tr_face4>(file));
        log_file(activity, file, std::format("Read {} rectangles", room.data.rectangles.size()));
    }

    void read_room_reverb_info(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading reverb info");
        room.reverb_info = read<uint8_t>(file);
        log_file(activity, file, std::format("Read reverb info: {}", room.reverb_info));
    }

    void read_room_sectors(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading number of z sectors");
        room.num_z_sectors = read<uint16_t>(file);
        log_file(activity, file, std::format("There are {} z sectors", room.num_z_sectors));
        log_file(activity, file, "Reading number of x sectors");
        room.num_x_sectors = read<uint16_t>(file);
        log_file(activity, file, std::format("There are {} x sectors", room.num_x_sectors));
        log_file(activity, file, std::format("Reading {} sectors", room.num_z_sectors * room.num_x_sectors));
        room.sector_list = read_vector<tr_room_sector>(file, room.num_z_sectors * room.num_x_sectors);
        log_file(activity, file, std::format("Read {} sectors", room.sector_list.size()));
    }

    void read_room_sprites(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading sprites");
        room.data.sprites = read_vector<int16_t, tr_room_sprite>(file);
        log_file(activity, file, std::format("Read {} sprites", room.data.sprites.size()));
    }

    void read_room_static_meshes(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading static meshes");
        room.static_meshes = read_vector<uint16_t, tr3_room_staticmesh>(file);
        log_file(activity, file, std::format("Read {} static meshes", room.static_meshes.size()));
    }

    void read_room_triangles(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading triangles");
        room.data.triangles = convert_triangles(read_vector<int16_t, tr_face3>(file));
        log_file(activity, file, std::format("Read {} triangles", room.data.triangles.size()));
    }

    void read_room_vertices_tr3_4(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading vertices");
        room.data.vertices = convert_vertices(read_vector<int16_t, tr3_room_vertex>(file));
        log_file(activity, file, std::format("Read {} vertices", room.data.vertices.size()));
    }

    void read_room_water_scheme(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading water scheme");
        room.water_scheme = read<uint8_t>(file);
        log_file(activity, file, std::format("Read water scheme: {}", room.water_scheme));
    }

    std::vector<uint32_t> read_sample_indices(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading sample indices");
        log_file(activity, file, "Reading sample indices");
        auto sample_indices = read_vector<uint32_t, uint32_t>(file);
        log_file(activity, file, std::format("Read {} sample indices", sample_indices.size()));
        return sample_indices;
    }

    std::vector<uint8_t> read_sound_data(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading sound data");
        log_file(activity, file, "Reading sound data");
        const auto sound_data = read_vector<int32_t, uint8_t>(file);
        log_file(activity, file, std::format("Read {} sound data", sound_data.size()));
        return sound_data;
    }

    std::vector<tr_x_sound_details> read_sound_details(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading sound details");
        log_file(activity, file, "Reading sound details");
        auto sound_details = read_vector<uint32_t, tr_x_sound_details>(file);
        log_file(activity, file, std::format("Read {} sound details", sound_details.size()));
        return sound_details;
    }

    std::vector<int16_t> read_sound_map(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading sound map");
        log_file(activity, file, "Reading sound map");
        std::vector<int16_t> sound_map;
        uint32_t next = peek<uint32_t>(file);
        while (next >= 65535)
        {
            sound_map.push_back(read<int16_t>(file));
            next = peek<uint32_t>(file);
        }
        log_file(activity, file, "Read sound map");
        return sound_map;
    }

    void read_sound_samples_tr4_5(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        uint32_t num_samples = read<uint32_t>(file);
        callbacks.on_progress("Reading sound samples");
        log_file(activity, file, std::format("Reading {} sound samples", num_samples));
        std::vector<std::vector<uint8_t>> samples;
        for (uint32_t i = 0; i < num_samples; ++i)
        {
            uint32_t uncompressed = read<uint32_t>(file);
            uncompressed;
            uint32_t compressed = read<uint32_t>(file);
            callbacks.on_sound(static_cast<uint16_t>(i), read_vector<uint8_t>(file, compressed));
        }
        log_file(activity, file, std::format("Read {} sound samples", num_samples));
    }

    std::vector<tr_sound_source> read_sound_sources(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading sound sources");
        log_file(activity, file, "Reading sound sources");
        const auto sound_sources = read_vector<uint32_t, tr_sound_source>(file);
        log_file(activity, file, std::format("Read {} sound sources", sound_sources.size()));
        return sound_sources;
    }

    std::vector<tr_sprite_sequence> read_sprite_sequences(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading sprite sequences");
        log_file(activity, file, "Reading sprite sequences");
        const auto sprite_sequences = read_vector<uint32_t, tr_sprite_sequence>(file);
        log_file(activity, file, std::format("Read {} sprite sequences", sprite_sequences.size()));
        return sprite_sequences;
    }

    std::vector<tr_sprite_texture> read_sprite_textures(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading sprite textures");
        log_file(activity, file, "Reading sprite textures");
        auto sprite_textures = read_vector<uint32_t, tr_sprite_texture>(file);
        log_file(activity, file, std::format("Read {} sprite textures", sprite_textures.size()));
        return sprite_textures;
    }

    std::vector<tr_state_change> read_state_changes(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading state changes");
        log_file(activity, file, "Reading state changes");
        const auto state_changes = read_vector<uint32_t, tr_state_change>(file);
        log_file(activity, file, std::format("Read {} state changes", state_changes.size()));
        return state_changes;
    }

    std::unordered_map<uint32_t, tr_staticmesh> read_static_meshes(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading static meshes");
        log_file(activity, file, "Reading static meshes");
        auto static_meshes = read_vector<uint32_t, tr_staticmesh>(file);
        log_file(activity, file, std::format("Read {} static meshes", static_meshes.size()));
        std::unordered_map<uint32_t, tr_staticmesh> mesh_map;
        for (const auto& mesh : static_meshes)
        {
            mesh_map.insert({ mesh.ID, mesh });
        }
        return mesh_map;
    }

    uint32_t read_textiles(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading textiles");
        log_file(activity, file, "Reading textiles");

        uint32_t num_textiles = read<uint32_t>(file);
        callbacks.on_progress(std::format("Skipping {} 8-bit textiles", num_textiles));
        log_file(activity, file, std::format("Skipping {} 8-bit textiles", num_textiles));
        skip(file, sizeof(tr_textile8) * num_textiles);

        callbacks.on_progress(std::format("Reading {} 16-bit textiles", num_textiles));
        log_file(activity, file, std::format("Reading {} 16-bit textiles", num_textiles));
        stream_vector<tr_textile16>(file, num_textiles, [&](auto&& t) { callbacks.on_textile(convert_textile(t)); });
        return num_textiles;
    }

    uint32_t read_textiles_tr4_5(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        log_file(activity, file, "Reading textile counts");
        uint16_t num_room_textiles = read<uint16_t>(file);
        uint16_t num_obj_textiles = read<uint16_t>(file);
        uint16_t num_bump_textiles = read<uint16_t>(file);
        log_file(activity, file, std::format("Textile counts - Room:{}, Object:{}, Bump:{}", num_room_textiles, num_obj_textiles, num_bump_textiles));
        const auto num_textiles = num_room_textiles + num_obj_textiles + num_bump_textiles;

        callbacks.on_progress(std::format("Reading {} 32-bit textiles", num_textiles));
        log_file(activity, file, std::format("Reading {} 32-bit textiles", num_textiles));
        auto textile32 = read_vector_compressed<tr_textile32>(file, num_textiles);

        constexpr auto is_blank = [](const auto& t)
            {
                for (const uint32_t& v : t.Tile)
                {
                    if (v)
                    {
                        return false;
                    }
                }
                return true;
            };

        if (std::all_of(textile32.begin(), textile32.end(), is_blank))
        {
            activity.log(trview::Message::Status::Warning, "32-bit textiles were all blank, discarding");
            textile32 = {};
            callbacks.on_progress(std::format("Reading {} 16-bit textiles", num_textiles));
            log_file(activity, file, std::format("Reading {} 16-bit textiles", num_textiles));
            auto textile16 = read_vector_compressed<tr_textile16>(file, num_textiles);

            for (const auto& textile : textile16)
            {
                callbacks.on_textile(convert_textile(textile));
            }
        }
        else
        {
            for (const auto& textile : textile32)
            {
                callbacks.on_textile(convert_textile(textile));
            }
            textile32 = {};

            callbacks.on_progress(std::format("Skipping {} 16-bit textiles", num_textiles));
            log_file(activity, file, std::format("Skipping {} 16-bit textiles", num_textiles));
            skip_vector_compressed(file);
        }

        log_file(activity, file, "Reading misc textiles");
        const auto textile32_misc = read_vector_compressed<tr_textile32>(file, 2);
        for (const auto& textile : textile32_misc)
        {
            callbacks.on_textile(convert_textile(textile));
        }
        return num_textiles;
    }

    void read_zones(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks, uint32_t num_boxes)
    {
        callbacks.on_progress("Reading zones");
        log_file(activity, file, "Reading zones");
        std::vector<int16_t> zones = read_vector<int16_t>(file, num_boxes * 10);
        log_file(activity, file, std::format("Read {} zones", zones.size()));
    }
}
