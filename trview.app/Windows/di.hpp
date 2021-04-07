#pragma once

#include <external/boost/di.hpp>

#include <trview.graphics/IDeviceWindow.h>

#include "ItemsWindow.h"
#include "TriggersWindow.h"
#include "ItemsWindowManager.h"
#include "TriggersWindowManager.h"
#include "RouteWindowManager.h"
#include "RoomsWindowManager.h"

namespace trview
{
    auto register_app_windows_module() noexcept
    {
        using namespace boost;
        using namespace graphics;

        return di::make_injector(
            di::bind<IItemsWindow::Source>.to(
            [](const auto& injector) -> IItemsWindow::Source
            {
                return [&](auto window)
                {
                    return std::make_shared<ItemsWindow>(
                        injector.create<IDeviceWindow::Source>(),
                        injector.create<ui::render::IRenderer::Source>(),
                        window);
                };
            }),
            di::bind<IItemsWindowManager>.to<ItemsWindowManager>(),
            di::bind<ITriggersWindow::Source>.to(
                [](const auto& injector) -> ITriggersWindow::Source
                {
                    return [&](auto window)
                    {
                        return std::make_shared<TriggersWindow>(
                            injector.create<IDeviceWindow::Source>(),
                            injector.create<ui::render::IRenderer::Source>(),
                            window);
                    };
                }),
            di::bind<ITriggersWindowManager>.to<TriggersWindowManager>(),
            di::bind<IRouteWindow::Source>.to(
                [](const auto& injector) -> IRouteWindow::Source
                {
                    return [&](auto window)
                    {
                        return std::make_shared<RouteWindow>(
                            injector.create<IDeviceWindow::Source>(),
                            injector.create<ui::render::IRenderer::Source>(),
                            window);
                    };
                }
            ),
            di::bind<IRouteWindowManager>.to<RouteWindowManager>(),
            di::bind<IRoomsWindow::Source>.to(
                [](const auto& injector) -> IRoomsWindow::Source
                {
                    return [&](auto window)
                    {
                        return std::make_shared<RoomsWindow>(
                            injector.create<IDeviceWindow::Source>(),
                            injector.create<ui::render::IRenderer::Source>(),
                            injector.create<ui::render::IMapRenderer::Source>(),
                            window);
                    };
                }),
            di::bind<IRoomsWindowManager>.to<RoomsWindowManager>(),
            di::bind<IViewer>.to<Viewer>()
        );
    }
}
