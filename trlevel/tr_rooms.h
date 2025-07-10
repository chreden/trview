#pragma once

#include "trtypes.h"
#include "tr_lights.h"
#include "tr_mesh.h"

namespace trlevel
{
    struct tr3_room_data
    {
        std::vector<trview_vertex> vertices;
        std::vector<trview_mesh_face4> rectangles;
        std::vector<trview_mesh_face3> triangles;
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
        int16_t ambient_intensity_1{ 0 };
        int16_t ambient_intensity_2{ 0 };
        int16_t light_mode{ 0 };

        std::vector<tr_x_room_light> lights;

        std::vector<tr3_room_staticmesh> static_meshes;

        int16_t alternate_room{ -1 };
        int16_t flags{ 0 };

        uint16_t water_scheme{ 0 };
        uint8_t reverb_info{ 0 };
        uint8_t alternate_group{ 0xff };
    };
}