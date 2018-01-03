#pragma once

#include <cstdint>
#include <vector>

namespace trlevel
{
    struct tr_colour
    {
        uint8_t Red;
        uint8_t Green;
        uint8_t Blue;
    };

    struct tr_colour4
    {
        uint8_t Red;
        uint8_t Green;
        uint8_t Blue;
        uint8_t Unused;
    };

    struct tr_textile8
    {
        uint8_t Tile[256 * 256];
    };

    struct tr_textile16
    {
        uint16_t Tile[256 * 256];
    };

    struct tr_textile32
    {
        uint32_t Tile[256 * 256];
    };

    struct tr_room_info
    {
        int32_t x;
        int32_t z;
        int32_t yBottom;
        int32_t yTop;
    };

    struct tr_vertex
    {
        int16_t x;
        int16_t y;
        int16_t z;
    };

    // Four vertices(the values are indices into the appropriate vertex list) and a texture(an index into 
    // the object - texture list) or colour(index into 8 - bit palette or 16 - bit palette).If the rectangle 
    // is a coloured polygon(not textured), the.Texture element contains two indices : the low byte(Texture & 0xFF)
    // is an index into the 256 - colour palette, while the high byte(Texture >> 8) is in index into the 16 - bit palette, 
    // when present. A textured rectangle will have its vertices mapped onto all 4 vertices of an object texture, 
    // in appropriate correspondence.
    // Texture field can have the bit 15 set: when it is, the face is double - sided(i.e.visible from both sides).
    // 1, 2, 3: If the rectangle is a coloured polygon(not textured), the.Texture element contains two indices : 
    // the low byte(Texture & 0xFF) is an index into the 256 - colour palette, while the high byte(Texture >> 8) is 
    // in index into the 16 - bit palette, when present.
    struct tr_face4
    {
        uint16_t vertices[4];
        uint16_t texture;
    };

    struct tr_face3
    {
        uint16_t vertices[3];
        uint16_t texture;
    };

    struct tr_room_sprite
    {
        int16_t vertex;
        int16_t texture;
    };

    struct tr3_room_vertex
    {
        tr_vertex   vertex;
        int16_t     lighting;
        uint16_t    attributes;
        uint16_t    colour;
    };

    struct tr3_room_data
    {
        std::vector<tr3_room_vertex> vertices;
        std::vector<tr_face4> rectangles;
        std::vector<tr_face3> triangles;
        std::vector<tr_room_sprite> sprites;
    };

    struct tr_room_portal
    {
        uint16_t  adjoining_room;
        tr_vertex normal;
        tr_vertex vertices[4];
    };

    struct tr_room_sector
    {
        uint16_t floordata_index;    // Index into FloorData[]
        uint16_t box_index;   // Index into Boxes[] (-1 if none)
        uint8_t  room_below;  // 255 is none
        int8_t   floor;      // Absolute height of floor
        uint8_t  room_above;  // 255 if none
        int8_t   ceiling;    // Absolute height of ceiling
    };

    struct tr3_room_light   // 24 bytes
    {
        // Position of light, in world coordinates
        int32_t x;
        int32_t y;
        int32_t z;       
        tr_colour4 colour;        // Colour of the light
        uint32_t   intensity;
        uint32_t   fade;          // Falloff value
    };

    struct tr3_room_staticmesh 
    {
        uint32_t x, y, z;    // Absolute position in world coordinates
        uint16_t rotation;
        uint16_t colour;     // 15-bit colour
        uint16_t unused;     // Not used!
        uint16_t mesh_id;     // Which StaticMesh item to draw
    };

    struct tr3_room
    {
        tr_room_info info;
        tr3_room_data data;

        std::vector<tr_room_portal> portals;

        uint16_t num_z_sectors;
        uint16_t num_x_sectors;
        std::vector<tr_room_sector> sector_list;

        int16_t ambient_intensity_1;
        int16_t ambient_intensity_2;

        std::vector<tr3_room_light> lights;

        std::vector<tr3_room_staticmesh> static_meshes;

        int16_t alternate_flags;
        int16_t flags;

        uint8_t water_scheme;
        uint8_t reverb_info;
        uint8_t filler;
    };

    // Convert a 16 bit textile into a 32 bit argb value.
    uint32_t convert_textile16(uint16_t t);
}