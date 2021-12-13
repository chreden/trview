#pragma once

#include "trtypes.h"
#include "tr_lights.h"

namespace trlevel
{
    struct tr3_room_data
    {
        std::vector<tr3_room_vertex> vertices;
        std::vector<tr4_mesh_face4> rectangles;
        std::vector<tr4_mesh_face3> triangles;
        std::vector<tr_room_sprite> sprites;
    };

    struct tr3_room
    {
        tr_room_info info;
        tr3_room_data data;

        std::vector<tr_room_portal> portals;

        uint16_t num_z_sectors{ 0u };
        uint16_t num_x_sectors{ 0u };
        std::vector<tr_room_sector> sector_list;

        uint32_t colour{ 0xffffffff };
        int16_t ambient_intensity_1;
        int16_t ambient_intensity_2;
        int16_t light_mode;

        std::vector<tr_x_room_light> lights;

        std::vector<tr3_room_staticmesh> static_meshes;

        int16_t alternate_room{ -1 };
        int16_t flags;

        uint16_t water_scheme;
        uint8_t reverb_info;
        uint8_t alternate_group{ 0xff };

        uint32_t room_colour;
    };
}