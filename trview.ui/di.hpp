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
                            auto mouse = injector.create<input::IMouse::Source>()(window);
                            return std::make_unique<Input>(
                                window,
                                control,
                                injector.create<std::shared_ptr<IShortcuts>>(),
                                injector.create<std::shared_ptr<IClipboard>>(),
                                mouse);
                        };
                    })
            );
        }
    }
}

