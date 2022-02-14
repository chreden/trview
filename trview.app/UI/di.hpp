#pragma once

#include <external/boost/di.hpp>
#include "ViewerUI.h"
#include "SettingsWindow.h"
#include "ViewOptions.h"
#include "ContextMenu.h"
#include "CameraControls.h"
#include "MapRenderer.h"
#include <trview.common/Resources.h>
#include <trview.graphics/IDevice.h>
#include "DX11ImGuiBackend.h"

namespace trview
{
    auto register_app_ui_module() noexcept
    {
        using namespace boost;
        return di::make_injector(
            di::bind<ISettingsWindow>.to<SettingsWindow>(),
            di::bind<ICameraControls>.to<CameraControls>(),
            di::bind<IViewOptions>.to<ViewOptions>(),
            di::bind<IContextMenu>.to<ContextMenu>(),
            di::bind<IImGuiBackend>.to<DX11ImGuiBackend>(),
            di::bind<IMapRenderer::Source>.to(
                [](const auto& injector) -> IMapRenderer::Source
                {
                    return [&](auto size)
                    {
                        return std::make_unique<MapRenderer>(
                            injector.create<std::shared_ptr<graphics::IDevice>>(),
                            *injector.create<std::shared_ptr<graphics::IFontFactory>>(),
                            size,
                            injector.create<graphics::ISprite::Source>(),
                            injector.create<graphics::IRenderTarget::SizeSource>());
                    };
                }),
            di::bind<IViewerUI>.to(
                [](const auto& injector) -> std::unique_ptr<IViewerUI>
                {
                    return std::make_unique<ViewerUI>(
                        injector.create<Window>(),
                        injector.create<std::shared_ptr<ITextureStorage>>(),
                        injector.create<std::shared_ptr<IShortcuts>>(),
                        injector.create<IMapRenderer::Source>(),
                        injector.create<std::unique_ptr<ISettingsWindow>>(),
                        injector.create<std::unique_ptr<IViewOptions>>(),
                        injector.create<std::unique_ptr<IContextMenu>>(),
                        injector.create<std::unique_ptr<ICameraControls>>());
                })
        );
    }
}
