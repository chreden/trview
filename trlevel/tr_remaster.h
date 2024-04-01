#pragma once

namespace trlevel
{
    namespace remaster
    {
        struct Vector3
        {
            short x;
            short y;
            short z;
        };

        struct Vector2
        {
            uint16_t u;
            uint16_t v;
        };


        struct UV
        {
            uint8_t x_frac;
            uint8_t x_whole;
            uint8_t y_frac;
            uint8_t y_whole;
        };

#pragma pack(push,1)
        struct Vertex
        {
            Vector3 position;
            char    unknown_x[5];
            char    texture_index;
            char    unknown_1[2];
            uint16_t u;
            char    unknown_2[2];
            uint16_t v;

            static int offset_u;
            static int offset_v;
            static int size;
            static float divisor;
        };
#pragma pack(pop)

        static_assert(sizeof(Vertex) == 20);

        struct Triangle
        {
            uint32_t vertices[3];
            uint32_t texture;
        };

        struct RoomData
        {
            std::vector<Vertex>     vertices;
            std::vector<Triangle>   triangles;
        };

        struct tr_remastered_textile
        {
            std::vector<std::string> paths;
        };
    }
}
