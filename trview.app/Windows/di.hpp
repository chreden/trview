#pragma once

#include <external/boost/di.hpp>

#include <trview.graphics/IDeviceWindow.h>

#include "ItemsWindow.h"
#include "TriggersWindow.h"
#include "ItemsWindowManager.h"
#include "TriggersWindowManager.h"
#include "RouteWindowManager.h"
#include "RoomsWindowManager.h"
#include "LightsWindowManager.h"
#include "LightsWindow.h"
#include "Viewer.h"

namespace trview
{
    auto register_app_windows_module() noexcept
    {
        using namespace boost;
        using namespace graphics;

        return di::make_injector(
            di::bind<IItemsWindow>.to<ItemsWindow>().in(di::unique),
            di::bind<IItemsWindow::Source>.to(
            [](const auto& injector) -> IItemsWindow::Source
            {
                return [&]()
                {
                    return injector.create<std::shared_ptr<IItemsWindow>>();
                };
            }),
            di::bind<IItemsWindowManager>.to<ItemsWindowManager>(),
            di::bind<ITriggersWindow>.to<TriggersWindow>().in(di::unique),
            di::bind<ITriggersWindow::Source>.to(
                [](const auto& injector) -> ITriggersWindow::Source
                {
                    return [&]()
                    {
                        return injector.create<std::shared_ptr<ITriggersWindow>>();
                    };
                }),
            di::bind<ITriggersWindowManager>.to<TriggersWindowManager>(),
            di::bind<IRouteWindow::Source>.to(
                [](const auto& injector) -> IRouteWindow::Source
                {
                    return [&]()
                    {
                        return std::make_shared<RouteWindow>(
                            injector.create<Window>(),
                            injector.create<std::shared_ptr<IClipboard>>(),
                            injector.create<std::shared_ptr<IDialogs>>(),
                            injector.create<std::shared_ptr<IFiles>>());
                    };
                }
            ),
            di::bind<IRouteWindowManager>.to<RouteWindowManager>(),
            di::bind<IRoomsWindow>.to<RoomsWindow>().in(di::unique),
            di::bind<IRoomsWindow::Source>.to(
                [](const auto& injector) -> IRoomsWindow::Source
                {
                    return [&]()
                    {
                        return injector.create<std::shared_ptr<IRoomsWindow>>();
                    };
                }),
            di::bind<IRoomsWindowManager>.to<RoomsWindowManager>(),
            di::bind<ILightsWindow>.to<LightsWindow>().in(di::unique),
            di::bind<ILightsWindow::Source>.to(
                [](const auto& injector) -> ILightsWindow::Source
                {
                    return [&]()
                    {
                        return injector.create<std::shared_ptr<ILightsWindow>>();
                    };
                }),
            di::bind<ILightsWindowManager>.to<LightsWindowManager>(),
            di::bind<IViewer>.to<Viewer>()
        );
    }
}
