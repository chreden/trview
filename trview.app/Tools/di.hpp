#pragma once

#include <external/boost/di.hpp>
#include "Compass.h"
#include "Measure.h"

namespace trview
{
    auto register_app_tools_module()
    {
        using namespace boost;
        return di::make_injector(
            di::bind<ICompass>.to<Compass>(),
            di::bind<IMeasure>.to<Measure>()
        );
    }
}
