#pragma once

#include <external/boost/di.hpp>
#include "WindowTester.h"
#include "Mouse.h"

namespace trview
{
    namespace input
    {
        auto register_module() noexcept
        {
            using namespace boost;
            return di::make_injector(
                di::bind<IWindowTester>.to<WindowTester>(),
                di::bind<IWindowTester::Source>.to([](const auto& injector) -> IWindowTester::Source
                    {
                        return [&](auto&& window)
                        {
                            return std::make_unique<WindowTester>(window);
                        };
                    }),
                di::bind<IMouse>.to<Mouse>(),
                di::bind<IMouse::Source>.to([](const auto& injector) -> IMouse::Source
                    {
                        return [&](auto&& window)
                        {
                            return std::make_shared<Mouse>(window,
                                injector.create<IWindowTester::Source>()(window));
                        };
                    })
            );
        }
    }
}
