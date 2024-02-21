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

        struct Vertex
        {
            Vector3 position;
            char    unknown[14];
        };

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
    }
}
