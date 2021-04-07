#pragma once

#include <external/boost/di.hpp>
#include "LevelLoader.h"

namespace trlevel
{
    auto register_module() noexcept
    {
        using namespace boost;
        return di::make_injector(
            di::bind<ILevelLoader>.to<LevelLoader>()
        );
    }
}
