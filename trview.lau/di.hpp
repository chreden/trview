#pragma once

#include <external/boost/di.hpp>
#include "DrmLoader.h"

namespace trview
{
    namespace lau
    {
        auto register_module() noexcept
        {
            using namespace boost;
            return di::make_injector(
                di::bind<IDrmLoader>.to<DrmLoader>());
        }
    }
}
