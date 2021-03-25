#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <memory>
#include <trview.ui/Control.h>

#include <trview.graphics/IDevice.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/IFontFactory.h>

namespace trview
{
    namespace ui
    {
        namespace render
        {
            struct IRenderer
            {
                using RendererSource = std::function<std::unique_ptr<IRenderer>(const Size&)>;

                virtual ~IRenderer() = 0;
                virtual void load(Control* control) = 0;
                virtual void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context) = 0;
                virtual void set_host_size(const Size& size) = 0;
            };
        }
    }
}
