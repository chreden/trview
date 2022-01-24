#pragma once

#include <external/boost/di.hpp>
#include "ViewerUI.h"
#include "SettingsWindow.h"
#include "ViewOptions.h"
#include "Bubble.h"
#include "ContextMenu.h"
#include "CameraControls.h"
#include <trview.common/Resources.h>
#include <trview.ui/ILoader.h>

namespace trview
{
    auto register_app_ui_module() noexcept
    {
        using namespace boost;
        return di::make_injector(
            di::bind<ISettingsWindow>.to<SettingsWindow>(),
            di::bind<ICameraControls>.to<CameraControls>(),
            di::bind<IViewOptions>.to<ViewOptions>(),
            di::bind<IBubble::Source>.to(
                [](const auto&) -> IBubble::Source
                {
                    return [&](ui::Control& parent)
                    {
                        return std::make_unique<Bubble>(parent);
                    };
                }),
            di::bind<IContextMenu>.to<ContextMenu>(),
            di::bind<IViewerUI>.to(
                [](const auto& injector) -> std::unique_ptr<IViewerUI>
                {
                    return std::make_unique<ViewerUI>(
                        injector.create<Window>(),
                        injector.create<std::shared_ptr<ITextureStorage>>(),
                        injector.create<std::shared_ptr<IShortcuts>>(),
                        injector.create<ui::IInput::Source>(),
                        injector.create<ui::render::IRenderer::Source>(),
                        injector.create<ui::render::IMapRenderer::Source>(),
                        injector.create<std::unique_ptr<ISettingsWindow>>(),
                        injector.create<std::unique_ptr<IViewOptions>>(),
                        injector.create<std::unique_ptr<IContextMenu>>(),
                        injector.create<std::unique_ptr<ICameraControls>>(),
                        injector.create<std::shared_ptr<ui::ILoader>>());
                })
        );
    }
}
