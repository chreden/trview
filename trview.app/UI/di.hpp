#pragma once

#include <external/boost/di.hpp>
#include "ViewerUI.h"
#include "SettingsWindow.h"
#include "ViewOptions.h"
#include "Bubble.h"
#include "ContextMenu.h"
#include <trview.common/Resources.h>
#include <trview.ui/json.h>

namespace trview
{
    auto register_app_ui_module() noexcept
    {
        using namespace boost;
        return di::make_injector(
            di::bind<ui::UiSource>.to(
                [](const auto&) -> ui::UiSource
                {
                    return [&](uint32_t resource)
                    {
                        auto data = get_resource_memory(resource, L"TEXT");
                        return ui::load_from_json(std::string(data.data, data.data + data.size));
                    };
                }),
            di::bind<ISettingsWindow::Source>.to(
                [](const auto& injector) -> ISettingsWindow::Source
                {
                    return [&](ui::Control& parent)
                    {
                        return std::make_unique<SettingsWindow>(parent, injector.create<ui::UiSource>());
                    };
                }),
            di::bind<IViewOptions::Source>.to(
                [](const auto&) -> IViewOptions::Source
                {
                    return [&](auto&& parent)
                    {
                        return std::make_unique<ViewOptions>(parent);
                    };
                }),
            di::bind<IBubble::Source>.to(
                [](const auto&) -> IBubble::Source
                {
                    return [&](ui::Control& parent)
                    {
                        return std::make_unique<Bubble>(parent);
                    };
                }),
            di::bind<IContextMenu::Source>.to(
                [](const auto&) -> IContextMenu::Source
                {
                    return [&](ui::Control& parent)
                    {
                        return std::make_unique<ContextMenu>(parent);
                    };
                }),
            di::bind<IViewerUI>.to<ViewerUI>()
        );
    }
}
