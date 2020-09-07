#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include <vector>

namespace trview
{
    namespace lau
    {
        struct Vertex
        {
            int16_t x;
            int16_t y;
            int16_t z;
        };

        struct Drm
        {
            uint32_t version;
            std::vector<Vertex> world_mesh;
        };

        std::unique_ptr<Drm> load_drm(const std::wstring& filename);
    }
}
