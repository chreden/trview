#pragma once

#include <cstdint>

namespace trview
{
    class Mesh;

    struct IMeshStorage
    {
        virtual ~IMeshStorage() = 0;

        virtual Mesh* mesh(uint32_t index) const = 0;
    };
}
