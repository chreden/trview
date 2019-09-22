#pragma once

#include <cstdint>

namespace trview
{
    class Mesh;

    struct IMeshStorage
    {
        virtual ~IMeshStorage() = 0;

        virtual Mesh* mesh(uint32_t mesh_pointer) const = 0;
    };
}
