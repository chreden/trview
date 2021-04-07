#pragma once

#include <external/boost/di.hpp>
#include "ViewerUI.h"

namespace trview
{
    auto register_app_ui_module()
    {
        using namespace boost;
        return di::make_injector(
            di::bind<IViewerUI>.to<ViewerUI>()
        );
    }
}
