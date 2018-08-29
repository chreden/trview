#pragma once

#include <cstdint>
#include "trtypes.h"
#include "LevelVersion.h"

namespace trlevel
{
    // Interface that defines a level.
    struct ILevel
    {
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

        // Gets the number of textiles in the level.
        // Returns: The number of textiles.
        virtual uint32_t num_textiles() const = 0;

        // Gets the 8 bit textile with the specified index.
        // Returns: The textile for this index.
        virtual tr_textile8 get_textile8(uint32_t index) const = 0;

        // Gets the 16 bit textile with the specified index.
        // Returns: The textile for this index.
        virtual tr_textile16 get_textile16(uint32_t index) const = 0;

        // Get the 8 or 16 bit textile with the specified index.
        // Returns: The colours for this index.
        virtual std::vector<uint32_t> get_textile(uint32_t index) const = 0;

        // Gets the number of rooms in the level.
        // Returns: The number of rooms.
        virtual uint16_t num_rooms() const = 0;

        // Get the room at the specified index.
        // Returns: The room.
        virtual tr3_room get_room(uint16_t index) const = 0;

        // Get the number of object textures in the level.
        // Returns: The number of object textures.
        virtual uint32_t num_object_textures() const = 0;

        // Get one of the object texture in the level.
        // index: The index of the texture to get.
        // Returns: The object texture.
        virtual tr_object_texture get_object_texture(uint32_t index) const = 0;

        // Get the floor data at the specified index.
        // index: The index of the floor data to get.
        // Returns: The floor data.
        virtual uint16_t get_floor_data(uint32_t index) const = 0;

        // Returns entire floor data vector.
        // Returns: The floor data.
        virtual std::vector<std::uint16_t> get_floor_data_all() const = 0;

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
        virtual tr_staticmesh get_static_mesh(uint32_t index) const = 0;

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
        virtual tr_sprite_texture get_sprite_texture(uint32_t index) const = 0;

        /// Find the first entity with the specified type.
        /// @param type The type ID of the entity.
        /// @param entity The output entity.
        /// @returns Whether the entity was found.
        virtual bool find_first_entity_by_type(int16_t type, tr2_entity& entity) const = 0;

        /// Get the true mesh from a type id. For example Lara's skin.
        /// @param type The type id to check.
        /// @returns The mesh index for the type.
        virtual int16_t get_mesh_from_type_id(int16_t type) const = 0;
    };
}