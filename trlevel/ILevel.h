#pragma once

#include <cstdint>
#include <optional>
#include <memory>

#include "trtypes.h"
#include "tr_rooms.h"
#include "LevelVersion.h"

namespace trlevel
{
    // Interface that defines a level.
    struct ILevel
    {
        using Source = std::function<std::shared_ptr<ILevel>(const std::string&)>;

        virtual ~ILevel() = 0;

        // Get the entry from the 8 bit palette at the given index.
        // index: The 0-255 index into the palette.
        // Returns: The palette colour.
        virtual tr_colour get_palette_entry8(uint32_t index) const = 0 ;

        // Get the entry from the 16 bit palette at the given index.
        // index: The 0-255 index into the palette.
        // Returns: The palette colour.
        virtual tr_colour4 get_palette_entry_16(uint32_t index) const = 0;

        // Get the entry from the 8 or 16 bit palette at the given index.
        // index: The 0-255 index into the palette.
        // Returns: The palette colour.
        virtual tr_colour4 get_palette_entry(uint32_t index) const = 0;

        // Get the entry from the 8 or 16 bit palette. Will try to use the 16 bit palette
        // and will fall back to the 8 bit palette if the 16 bit palette is missing.
        // index8: The index into the 8 bit palette.
        // index16: The index into the 16 bit palette.
        // Returns: The palette colour.
        virtual tr_colour4 get_palette_entry(uint32_t index8, uint32_t index16) const = 0;

        // Gets the number of rooms in the level.
        // Returns: The number of rooms.
        virtual uint32_t num_rooms() const = 0;

        // Get the room at the specified index.
        // Returns: The room.
        virtual tr3_room get_room(uint32_t index) const = 0;

        // Get the number of object textures in the level.
        // Returns: The number of object textures.
        virtual uint32_t num_object_textures() const = 0;

        // Get one of the object texture in the level.
        // index: The index of the texture to get.
        // Returns: The object texture.
        virtual tr_object_texture get_object_texture(uint32_t index) const = 0;

        /// Get the number of floordata values in the level.
        /// @returns The number of floordata values.
        virtual uint32_t num_floor_data() const = 0;

        // Get the floor data at the specified index.
        // index: The index of the floor data to get.
        // Returns: The floor data.
        virtual uint16_t get_floor_data(uint32_t index) const = 0;

        // Returns entire floor data vector.
        // Returns: The floor data.
        virtual std::vector<std::uint16_t> get_floor_data_all() const = 0;

        /// Get the number of ai objects in the level.
        /// Returns: The number of ai objects.
        virtual uint32_t num_ai_objects() const = 0;

        // Get the ai object at the specified index.
        // index: The index of the ai object to get.
        // Returns: The ai object.
        virtual tr4_ai_object get_ai_object(uint32_t index) const = 0;

        // Get the number of entities in the level.
        // Returns: The number of entities.
        virtual uint32_t num_entities() const = 0;

        // Get the entity at the specified index.
        // index: The index of the entity to get.
        // Returns: The entity.
        virtual tr2_entity get_entity(uint32_t index) const = 0;

        // Get the number of models in the level.
        // Returns: The number of models.
        virtual uint32_t num_models() const = 0;

        // Get the model at the specfied index.
        // index: The index of the model to get.
        // Returns: The model.
        virtual tr_model get_model(uint32_t index) const = 0;

        // Get the model with the specified ID.
        // id: The id of the model.
        // model: The location to store the model.
        // Returns: Whether the model was found.
        virtual bool get_model_by_id(uint32_t id, tr_model& out) const = 0;

        // Get the number of static meshes in the level.
        // Returns: The number of models.
        virtual uint32_t num_static_meshes() const = 0;

        // Get the static mesh at the specfied index.
        // index: The index of the model to get.
        // Returns: The model.
        virtual std::optional<tr_staticmesh> get_static_mesh(uint32_t index) const = 0;

        /// Get the number of mesh pointers in the level.
        virtual uint32_t num_mesh_pointers() const = 0;

        // Get the mesh referenced by the specified mesh pointer.
        // mesh_pointer: The mesh pointer index.
        // Returns: The mesh.
        virtual tr_mesh get_mesh_by_pointer(uint32_t mesh_pointer) const = 0;

        // Get the mesh tree node at the specified index.
        // index: The starting mesh tree index.
        // node_count: The number of nodes to read.
        // Returns: The mesh tree node.
        virtual std::vector<tr_meshtree_node> get_meshtree(uint32_t starting_index, uint32_t node_count) const = 0;

        // Get the frame at the specified index. Read the specified number of meshes.
        // frame_offset: The frame offset.
        // mesh_count: The number of meshes to read.
        // Returns: The frame.
        virtual tr2_frame get_frame(uint32_t frame_offset, uint32_t mesh_count) const = 0;

        // Get the version of the game that the level was built for.
        // Returns: The level version.
        virtual LevelVersion get_version() const = 0;

        // Get the sprite squence with the specified ID.
        // sprite_sequence_id: The id of the sprite sequence to find.
        // sequence: The place to store the sequence.
        // Returns: Whether the sprite sequence was found.
        virtual bool get_sprite_sequence_by_id(int32_t sprite_sequence_id, tr_sprite_sequence& sequence) const = 0;

        // Get the sprite texture with the specified ID.
        // index: The index of the sprite texture to get.
        // Returns: The sprite texture.
        virtual std::optional<tr_sprite_texture> get_sprite_texture(uint32_t index) const = 0;

        /// Find the first entity with the specified type.
        /// @param type The type ID of the entity.
        /// @param entity The output entity.
        /// @returns Whether the entity was found.
        virtual bool find_first_entity_by_type(int16_t type, tr2_entity& entity) const = 0;

        /// Get the true mesh from a type id. For example Lara's skin.
        /// @param type The type id to check.
        /// @returns The mesh index for the type.
        virtual int16_t get_mesh_from_type_id(int16_t type) const = 0;

        virtual std::string name() const = 0;

        virtual uint32_t num_cameras() const = 0;
        virtual tr_camera get_camera(uint32_t index) const = 0;

        virtual Platform platform() const = 0;
        virtual PlatformAndVersion platform_and_version() const = 0;

        struct LoadCallbacks
        {
            std::function<void(const std::string&)> on_progress_callback;
            std::function<void(const std::vector<uint32_t>&)> on_textile_callback;
            std::function<void(uint16_t, const std::vector<uint8_t>&)> on_sound_callback;

            void on_progress(const std::string& message) const;
            void on_textile(const std::vector<uint32_t>& data) const;
            void on_sound(uint16_t id, const std::vector<uint8_t>&) const;
        };

        virtual void load(const LoadCallbacks& callbacks) = 0;
        virtual std::vector<tr_sound_source> sound_sources() const = 0;
        virtual std::vector<tr_x_sound_details> sound_details() const = 0;
        virtual std::vector<int16_t> sound_map() const = 0;
        virtual bool trng() const = 0;
    };
}