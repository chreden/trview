#pragma once

#include <external/boost/di.hpp>
#include "Input.h"

namespace trview
{
    namespace ui
    {
        auto register_module() noexcept
        {
            using namespace boost;
            return di::make_injector(
                di::bind<IInput::Source>.to(
                    [](const auto& injector) -> IInput::Source
                    {
                        return [&](auto&& window, auto&& control)
                        {
                            const auto shortcuts_source = injector.create<IShortcuts::Source>();
                            return std::make_unique<Input>(
                                window,
                                control,
                                shortcuts_source(window));
                        };
                    })
            );
        }
    }
}

