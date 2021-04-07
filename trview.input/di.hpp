#pragma once

#include <external/boost/di.hpp>
#include "WindowTester.h"
#include "Mouse.h"

namespace trview
{
    namespace input
    {
        auto register_module()
        {
            using namespace boost;
            return di::make_injector(
                di::bind<IWindowTester>.to<WindowTester>(),
                di::bind<IMouse>.to<Mouse>()
            );
        }
    }
}
