#include "DeviceWindow.h"
#include "Device.h"

using namespace Microsoft::WRL;

namespace trview
{
    namespace graphics
    {
        DeviceWindow::DeviceWindow(Device& device, const Window& window)
            : _device(device), _context(device.context())
        {
            // Swap chain description.
            DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
            swap_chain_desc.BufferCount = 1;
            swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            swap_chain_desc.BufferDesc.Height = static_cast<uint32_t>(window.size().height);
            swap_chain_desc.BufferDesc.Width = static_cast<uint32_t>(window.size().width);
            swap_chain_desc.BufferDesc.RefreshRate.Numerator = 1;
            swap_chain_desc.BufferDesc.RefreshRate.Denominator = 60;
            swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swap_chain_desc.OutputWindow = window.window();
            swap_chain_desc.Windowed = TRUE;
            swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
            swap_chain_desc.SampleDesc.Count = 1;
            swap_chain_desc.SampleDesc.Quality = 0;

            ComPtr<IDXGIDevice> dxgi_device;
            _device.device().As(&dxgi_device);
            ComPtr<IDXGIAdapter> dxgi_adapter;
            dxgi_device->GetAdapter(&dxgi_adapter);
            ComPtr<IDXGIFactory> factory;
            dxgi_adapter->GetParent(__uuidof(IDXGIFactory), &factory);
            factory->CreateSwapChain(_device.device().Get(), &swap_chain_desc, &_swap_chain);

            create_render_target();
        }

        // Create the render target view from the swap chain that has been created.
        void DeviceWindow::create_render_target()
        {
            ComPtr<ID3D11Texture2D> back_buffer;
            _swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(back_buffer.GetAddressOf()));
            _render_target = std::make_unique<graphics::RenderTarget>(_device, back_buffer, graphics::RenderTarget::DepthStencilMode::Enabled);
        }

        void DeviceWindow::begin()
        {
            _device.begin();
            _render_target->apply(_context);
        }

        void DeviceWindow::clear(const DirectX::SimpleMath::Color& colour)
        {
            _render_target->clear(_context, colour);
        }

        void DeviceWindow::resize()
        {
            _context->ClearState();
            _render_target.reset();

            _swap_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
            create_render_target();
        }

        void DeviceWindow::present(bool vsync)
        {
            _swap_chain->Present(vsync ? 1 : 0, 0);
        }

        Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceWindow::context() const
        {
            return _context;
        }
    }
}
