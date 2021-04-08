#pragma once

#include <external/boost/di.hpp>
#include "SettingsLoader.h"

namespace trview
{
    auto register_app_settings_module() noexcept
    {
        using namespace boost;
        return di::make_injector(
            di::bind<ISettingsLoader>.to<SettingsLoader>()
        );
    }
}