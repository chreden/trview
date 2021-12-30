#pragma once

#include "Drm.h"

namespace trview
{
    namespace lau
    {
        struct IDrmLoader
        {
            virtual ~IDrmLoader() = 0;
            virtual std::unique_ptr<Drm> load(const std::wstring filename) const = 0;
        };
    }
}
