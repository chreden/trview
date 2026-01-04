#pragma once

#include <string>
#include <array>
#include <vector>
#include <unordered_map>
#include <optional>

#include "ILevel.h"
#include "trtypes.h"
#include "IDecrypter.h"
#include "IHasher.h"

#include <trview.common/Logs/ILog.h>
#include <trview.common/Logs/Activity.h>
#include <trview.common/IFiles.h>

namespace trlevel
{
    class Level : public ILevel
    {
    public:
        explicit Level(const std::string& filename,
            const std::shared_ptr<IPack>& pack,
            const std::shared_ptr<trview::IFiles>& files,
            const std::shared_ptr<IDecrypter>& decrypter,
            const std::shared_ptr<trview::ILog>& log,
            const std::shared_ptr<IHasher>& hasher,
            const IPack::Source& pack_source);

        explicit Level(const std::string& filename,
            const std::shared_ptr<IPack>& pack,
            const std::shared_ptr<trview::IFiles>& files,
            const std::shared_ptr<IDecrypter>& decrypter,
            const std::shared_ptr<trview::ILog>& log,
            const std::shared_ptr<IHasher>& hasher);

        virtual ~Level();

        // Get the entry from the 8 bit palette at the given index.
        // index: The 0-255 index into the palette.
        // Returns: The palette colour.
        virtual tr_colour get_palette_entry8(uint32_t index) const override;

        // Get the entry from the 16 bit palette at the given index.
        // index: The 0-255 index into the palette.
        // Returns: The palette colour.
        virtual tr_colour4 get_palette_entry_16(uint32_t index) const override;

        // Get the entry from the 8 or 16 bit palette at the given index.
        // index: The 0-255 index into the palette.
        // Returns: The palette colour.
        virtual tr_colour4 get_palette_entry(uint32_t index) const override;

        // Get the entry from the 8 or 16 bit palette. Will try to use the 16 bit palette
        // and will fall back to the 8 bit palette if the 16 bit palette is missing.
        // index8: The index into the 8 bit palette.
        // index16: The index into the 16 bit palette.
        // Returns: The palette colour.
        virtual tr_colour4 get_palette_entry(uint32_t index8, uint32_t index16) const override;

        // Gets the number of rooms in the level.
        // Returns: The number of rooms.
        virtual uint32_t num_rooms() const override;

        // Get the room at the specified index.
        // Returns: The room.
        virtual tr3_room get_room(uint32_t index) const override;

        std::vector<tr_object_texture> object_textures() const override;

        /// Get the number of floordata values in the level.
        /// @returns The number of floordata values.
        virtual uint32_t num_floor_data() const override;

        // Get the floor data at the specified index.
        // index: The index of the floor data to get.
        // Returns: The floor data.
        virtual uint16_t get_floor_data(uint32_t index) const override;

        // Returns entire floor data vector.
        // Returns: The floor data.
        virtual std::vector<std::uint16_t> get_floor_data_all() const override; 

        virtual uint32_t num_ai_objects() const override;
        virtual tr4_ai_object get_ai_object(uint32_t index) const override;

        // Get the number of entities in the level.
        // Returns: The number of entities.
        virtual uint32_t num_entities() const override;

        // Get the entity at the specified index.
        // index: The index of the entity to get.
        // Returns: The entity.
        virtual tr2_entity get_entity(uint32_t index) const override;

        // Get the number of models in the level.
        // Returns: The number of models.
        virtual uint32_t num_models() const override;

        // Get the model at the specfied index.
        // index: The index of the model to get.
        // Returns: The model.
        virtual tr_model get_model(uint32_t index) const override;

        // Get the model with the specified ID.
        // id: The id of the model.
        // model: The location to store the model.
        // Returns: Whether the model was found.
        virtual bool get_model_by_id(uint32_t id, tr_model& model) const override;

        // Get the number of static meshes in the level.
        // Returns: The number of models.
        virtual uint32_t num_static_meshes() const override;

        // Get the static mesh at the specfied index.
        // index: The mesh ID of the model to get.
        // Returns: The model.
        virtual std::optional<tr_staticmesh> get_static_mesh(uint32_t mesh_id) const override;

        /// Get the number of mesh pointers in the level.
        virtual uint32_t num_mesh_pointers() const override;

        // Get the mesh at the specified index.
        // index: The index of the mesh to get.
        // Returns: The mesh.
        virtual tr_mesh get_mesh_by_pointer(uint32_t mesh_pointer) const override;

        // Get the mesh tree node at the specified index.
        // index: The mesh tree index.
        // node_count: The number of nodes to read.
        // Returns: The mesh tree node.
        virtual std::vector<tr_meshtree_node> get_meshtree(uint32_t starting_index, uint32_t node_count) const override;

        // Get the frame at the specified index. Read the specified number of meshes.
        // frame_offset: The frame offset.
        // mesh_count: The number of meshes to read.
        // Returns: The frame.
        virtual tr2_frame get_frame(uint32_t frame_offset, uint32_t mesh_count) const override;

        // Get the version of the game that the level was built for.
        // Returns: The level version.
        virtual LevelVersion get_version() const override;

        // Get the sprite squence with the specified ID.
        // sprite_sequence_id: The id of the sprite sequence to find.
        // sequence: The place to store the sequence.
        // Returns: Whether the sprite sequence was found.
        virtual bool get_sprite_sequence_by_id(int32_t sprite_sequence_id, tr_sprite_sequence& sequence) const override;

        // Get the sprite texture with the specified ID.
        // index: The index of the sprite texture to get.
        // Returns: The sprite texture.
        virtual std::optional<tr_sprite_texture> get_sprite_texture(uint32_t index) const override;

        /// Find the first entity with the specified type.
        /// @param type The type ID of the entity.
        /// @param entity The output entity.
        /// @returns Whether the entity was found.
        virtual bool find_first_entity_by_type(int16_t type, tr2_entity& entity) const override;

        virtual std::string name() const override;

        virtual uint32_t num_cameras() const override;
        virtual tr_camera get_camera(uint32_t index) const override;
        Platform platform() const override;
        void load(const LoadCallbacks& callbacks) override;
        std::vector<tr_sound_source> sound_sources() const override;
        std::vector<tr_x_sound_details> sound_details() const override;
        std::vector<int16_t> sound_map() const override;
        bool trng() const override;
        PlatformAndVersion platform_and_version() const override;
        std::weak_ptr<IPack> pack() const override;
        std::vector<tr4_flyby_camera> flyby_cameras() const override;
        std::vector<std::vector<int16_t>> animated_textures() const override;
        uint32_t animated_texture_uv_count() const override;
        std::string hash() const override;
        std::string filename() const override;
    private:
        void generate_meshes(const std::vector<uint16_t>& mesh_data);
        tr_colour4 colour_from_object_texture(uint32_t texture) const;
        uint16_t convert_textile4(uint16_t tile, uint16_t clut_id);
        uint16_t attribute_for_clut(uint16_t clut_id) const;

        // New level bits:
        void read_header(std::basic_ispanstream<uint8_t>& file, std::vector<uint8_t>& bytes, trview::Activity& activity, const LoadCallbacks& callbacks);
        void read_object_textures_tr1_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void read_object_textures_tr2_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void read_object_textures_tr3_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void read_palette_tr1(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void read_palette_tr2_3(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void read_room_textures_tr3_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void read_textiles_tr1_pc(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void read_textiles_tr2_pc_e3(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void read_textiles_tr1_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void read_textiles_tr1_psx_version_27(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void read_textiles_tr2_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void read_textiles_tr2_psx_version_44(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void read_textiles_tr2_psx_version_42(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void read_textiles_tr3_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void read_sprite_textures_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void read_sounds_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks, uint32_t sample_frequency);
        void read_sounds_tr1_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks, uint32_t sample_frequency);
        void read_sounds_tr4_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const ILevel::LoadCallbacks& callbacks, uint32_t start, const tr4_psx_level_info& info, uint32_t sample_frequency);
        void read_sounds_external_tr1_psx(trview::Activity& activity, const LoadCallbacks& callbacks);
        void read_sound_samples_tr4_5(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void read_sound_map_tr5_psx(std::basic_ispanstream<uint8_t>& file, const tr4_psx_level_info& info, trview::Activity& activity, const LoadCallbacks& callbacks);
        void adjust_room_textures_psx();
        void adjust_room_textures();

        void load_tr1_pc(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr1_pc_version_21(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr1_pc_may_1996(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr1_pc_may_1996_wad(std::vector<uint8_t>& textile_buffer, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr1_pc_may_1996_swd(std::vector<uint8_t>& textile_buffer, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr1_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr1_psx_version_27(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr1_psx_may_1996(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr1_saturn(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr1_saturn_sat(std::basic_ispanstream<uint8_t>& level_file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr1_saturn_sad(trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr1_saturn_snd(trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr1_saturn_spr(trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr2_pc(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr2_pc_e3(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr2_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr2_psx_version_38(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr2_psx_version_42(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr2_psx_version_44(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr3_pc(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr3_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr4_pc(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr4_pc_remastered(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr4_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr4_psx_version_111(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr4_psx_opsm_90(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr5_dc(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr5_pc(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr5_pc_remastered(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_tr5_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);
        void load_psx_pack(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks);

        void load_sound_fx(trview::Activity& activity, const LoadCallbacks& callbacks);
        std::optional<std::vector<uint8_t>> load_main_sfx();
        void load_ngle_sound_fx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const LoadCallbacks& callbacks);

        void generate_mesh(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream);
        void generate_mesh_tr1_pc_version_21(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream);
        void generate_mesh_tr1_pc_may_1996(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream);
        void generate_mesh_tr1_psx(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream);
        void generate_mesh_tr1_psx_may_1996(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream);
        void generate_mesh_tr1_saturn(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream);
        void generate_mesh_tr2_psx(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream);
        void generate_mesh_tr2_psx_version_44(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream);
        void generate_mesh_tr2_psx_version_38(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream);
        void generate_mesh_tr3_psx(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream);
        void generate_mesh_tr4_psx(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream);
        void generate_object_textures_tr4_psx(std::basic_ispanstream<uint8_t>& file, uint32_t start, const tr4_psx_level_info& info);
        void generate_sound_samples(const LoadCallbacks& callbacks);
        void generate_sounds(const LoadCallbacks& callbacks);
        void generate_textiles_from_textile8(const LoadCallbacks& callbacks);

        PlatformAndVersion _platform_and_version;

        std::vector<tr_colour>  _palette;
        std::vector<tr_colour4> _palette16;

        uint32_t                  _num_textiles{ 0u };
        std::vector<tr_textile4>  _textile4;
        std::vector<tr_textile8>  _textile8;
        std::vector<tr_textile16> _textile16;
        std::vector<tr_clut> _clut;
        std::vector<std::pair<uint16_t, uint16_t>> _converted_t16;

        std::vector<tr3_room>          _rooms;
        std::vector<tr_object_texture> _object_textures;
        std::vector<tr_object_texture_psx> _object_textures_psx;
        std::vector<std::vector<int16_t>> _animated_textures;
        uint32_t _animated_texture_uv_count{ 0u };
        std::vector<uint16_t>          _floor_data;
        std::vector<tr_model>          _models;
        std::vector<tr2_entity>        _entities;
        std::unordered_map<uint32_t, tr_staticmesh> _static_meshes;
        std::vector<tr4_ai_object>     _ai_objects;
        std::vector<tr4_animation>     _animations;
        std::vector<tr_state_change>   _state_changes;
        std::vector<tr_anim_dispatch>  _anim_dispatches;

        uint16_t _lara_type{ 0u };
        uint16_t _weather_type{ 0u };

        // Mesh management.
        std::unordered_map<uint32_t, tr_mesh> _meshes;
        std::vector<uint16_t>                 _mesh_data;
        std::vector<uint32_t>                 _mesh_pointers;
        std::vector<uint32_t>                 _meshtree;
        std::vector<uint16_t>                 _frames;
        std::vector<tr_sprite_texture>        _sprite_textures;
        std::vector<tr_sprite_sequence>       _sprite_sequences;

        std::string _name;

        std::vector<tr_camera> _cameras;
        std::vector<tr4_flyby_camera> _flyby_cameras;
        std::vector<tr_sound_source>    _sound_sources;
        std::vector<tr_x_sound_details> _sound_details;
        std::vector<int16_t> _sound_map;
        std::vector<uint32_t> _sample_indices;
        std::vector<uint8_t> _sound_data;
        std::vector<std::vector<uint8_t>> _sound_samples;

        std::shared_ptr<trview::ILog>   _log;
        std::shared_ptr<IDecrypter>     _decrypter;
        std::string                     _filename;
        std::shared_ptr<trview::IFiles> _files;
        bool                            _trng{ false };

        IPack::Source _pack_source;
        std::shared_ptr<IPack> _pack;
        std::shared_ptr<IHasher> _hasher;
        std::string _hash;
    };
}
