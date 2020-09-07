#pragma once

#include <memory>
#include <string>
#include <cstdint>

namespace trview
{
    namespace lau
    {
        struct Drm
        {
            uint16_t version;
        };

        std::unique_ptr<Drm> load_drm(const std::wstring& filename);
    }
}
