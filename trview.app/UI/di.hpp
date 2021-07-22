#pragma once

#include <external/boost/di.hpp>
#include "ViewerUI.h"
#include "SettingsWindow.h"
#include "Bubble.h"

namespace trview
{
    auto register_app_ui_module() noexcept
    {
        using namespace boost;
        return di::make_injector(
            di::bind<ISettingsWindow::Source>.to(
                [](const auto&) -> ISettingsWindow::Source
                {
                    return [&](ui::Control& parent)
                    {
                        return std::make_unique<SettingsWindow>(parent);
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
            di::bind<IViewerUI>.to<ViewerUI>()
        );
    }
}
