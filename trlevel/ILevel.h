#pragma once

#include <cstdint>
#include "trtypes.h"

namespace trlevel
{
    // Interface that defines a level.
    struct ILevel
    {
        virtual ~ILevel() = 0;

        // Get the entry from the 8 bit palette at the given index.
        // index: The 0-255 index into the palette.
        // Returns: The palette colour.
        virtual tr_colour get_palette_entry(uint32_t index) const = 0 ;

        // Get the entry from the 16 bit palette at the given index.
        // index: The 0-255 index into the palette.
        // Returns: The palette colour.
        virtual tr_colour4 get_palette_entry_16(uint32_t index) const = 0;

        // Gets the number of textiles in the level.
        // Returns: The number of textiles.
        virtual uint32_t num_textiles() const = 0;

        // Gets the 8 bit textile with the specified index.
        // Returns: The textile for this index.
        virtual tr_textile8 get_textile8(uint32_t index) const = 0;

        // Gets the 16 bit textile with the specified index.
        // Returns: The textile for this index.
        virtual tr_textile16 get_textile16(uint32_t index) const = 0;

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

        // Get the number of static meshes in the level.
        // Returns: The number of models.
        virtual uint32_t num_static_meshes() const = 0;

        // Get the static mesh at the specfied index.
        // index: The index of the model to get.
        // Returns: The model.
        virtual tr_staticmesh get_static_mesh(uint32_t index) const = 0;
    };
}