/// @file Device.h
/// @brief Wraps the D3D device.
/// 
/// Wraps the D3D device and manages common D3D operations.

#pragma once

#include <memory>
#include <wrl/client.h>
#include <d3d11.h>
#include <SimpleMath.h>

#include <trview.common/Window.h>

namespace trview
{
    namespace graphics
    {
        class RenderTarget;

        /// Wraps the D3D device and manages common D3D operations.
        class Device final
        {
        public:
            Device(const Window& window);
            ~Device();

            void begin();

            void clear(const DirectX::SimpleMath::Color& colour);

            const Microsoft::WRL::ComPtr<ID3D11Device>& device() const;

            const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context() const;

            void present();

            void resize();
        private:
            void create_render_target();

            Microsoft::WRL::ComPtr<IDXGISwapChain>      _swap_chain;
            Microsoft::WRL::ComPtr<ID3D11Device>        _device;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context;
            std::unique_ptr<graphics::RenderTarget>     _render_target;
        };
    }
}
