#pragma once

#include <external/boost/di.hpp>
#include "FileDropper.h"
#include "LevelSwitcher.h"
#include "RecentFiles.h"
#include "UpdateChecker.h"

namespace trview
{
    auto register_app_menus_module()
    {
        using namespace boost;
        return di::make_injector(
            di::bind<IUpdateChecker>.to<UpdateChecker>(),
            di::bind<IFileDropper>.to<FileDropper>(),
            di::bind<ILevelSwitcher>.to<LevelSwitcher>(),
            di::bind<IRecentFiles>.to<RecentFiles>()
        );
    }
}