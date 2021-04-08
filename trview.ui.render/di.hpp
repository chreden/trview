#pragma once

#include <external/boost/di.hpp>
#include "Renderer.h"
#include "MapRenderer.h"
#include <trview.graphics/IRenderTarget.h>

namespace trview
{
    namespace ui
    {
        namespace render
        {
            auto register_module() noexcept
            {
                using namespace graphics;
                using namespace boost;
                return di::make_injector(
                    di::bind<IRenderer::Source>.to(
                        [](const auto& injector) -> IRenderer::Source
                        {
                            return [&](auto size)
                            {
                                return std::make_unique<Renderer>(
                                    injector.create<std::shared_ptr<IDevice>>(),
                                    injector.create<IRenderTarget::SizeSource>(),
                                    *injector.create<std::shared_ptr<IFontFactory>>(),
                                    size,
                                    injector.create<ISprite::Source>());
                            };
                        }),
                    di::bind<ui::render::IMapRenderer::Source>.to(
                        [](const auto& injector) -> IMapRenderer::Source
                        {
                            return [&](auto size)
                            {
                                return std::make_unique<MapRenderer>(
                                    injector.create<std::shared_ptr<IDevice>>(),
                                    *injector.create<std::shared_ptr<IFontFactory>>(),
                                    size,
                                    injector.create<ISprite::Source>(),
                                    injector.create<IRenderTarget::SizeSource>());
                            };
                        }));
            }
        }
    }
}