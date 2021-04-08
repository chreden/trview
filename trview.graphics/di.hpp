#pragma once

#include <external/boost/di.hpp>
#include "Device.h"
#include "FontFactory.h"
#include "ShaderStorage.h"
#include "RenderTarget.h"
#include "DeviceWindow.h"
#include "Sprite.h"

namespace trview
{
    namespace graphics
    {
        auto register_module() noexcept
        {
            using namespace boost;

            return di::make_injector(
                di::bind<IDevice>.to<Device>(),
                di::bind<IFontFactory>.to<FontFactory>(),
                di::bind<IShaderStorage>.to<ShaderStorage>(),
                di::bind<IRenderTarget::SizeSource>.to(
                    [](const auto& injector) -> IRenderTarget::SizeSource
                    {
                        return [&](auto&& width, auto&& height, auto&& depth_stencil_mode)
                        {
                            return std::make_unique<RenderTarget>(
                                injector.create<std::shared_ptr<IDevice>>(),
                                width,
                                height,
                                depth_stencil_mode);
                        };
                    }),
                di::bind<IRenderTarget::TextureSource>.to(
                    [](const auto& injector) -> IRenderTarget::TextureSource
                    {
                        return [&](auto&& texture, auto&& depth_stencil_mode)
                        {
                            return std::make_unique<RenderTarget>(
                                injector.create<std::shared_ptr<IDevice>>(),
                                texture,
                                depth_stencil_mode);
                        };
                    }),
                di::bind<ISprite::Source>.to(
                    [](const auto& injector) -> ISprite::Source
                    {
                        return [&](auto size)
                        {
                            return std::make_unique<Sprite>(
                                injector.create<std::shared_ptr<IDevice>>(),
                                injector.create<std::shared_ptr<IShaderStorage>>(),
                                size);
                        };
                    }),
                di::bind<IDeviceWindow::Source>.to(
                    [](const auto& injector) -> IDeviceWindow::Source
                    {
                        return [&](auto&& window)
                        {
                            return std::make_unique<DeviceWindow>(
                                injector.create<std::shared_ptr<IDevice>>(),
                                injector.create<IRenderTarget::TextureSource>(),
                                window);
                        };
                    })
            );
        }
    }
}

