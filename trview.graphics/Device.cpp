#include "Device.h"
#include "RenderTarget.h"

using namespace Microsoft::WRL;

namespace trview
{
    namespace graphics
    {
        Device::Device(const Window& window)
        {
            // Swap chain description.
            DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
            swap_chain_desc.BufferCount = 1;
            swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            swap_chain_desc.BufferDesc.Height = window.height();
            swap_chain_desc.BufferDesc.Width = window.width();
            swap_chain_desc.BufferDesc.RefreshRate.Numerator = 1;
            swap_chain_desc.BufferDesc.RefreshRate.Denominator = 60;
            swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swap_chain_desc.OutputWindow = window.window();
            swap_chain_desc.Windowed = TRUE;
            swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
            swap_chain_desc.SampleDesc.Count = 1;
            swap_chain_desc.SampleDesc.Quality = 0;

            D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                D3D11_SDK_VERSION, &swap_chain_desc, &_swap_chain, &_device, nullptr, &_context);

            create_render_target();

            D3D11_BLEND_DESC desc;
            memset(&desc, 0, sizeof(desc));
            desc.RenderTarget[0].BlendEnable = true;
            desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            _device->CreateBlendState(&desc, &_blend_state);
        }

        Device::~Device()
        {
        }

        void Device::begin()
        {
            _render_target->apply(_context);
            _context->OMSetBlendState(_blend_state.Get(), 0, 0xffffffff);
        }

        void Device::clear(const DirectX::SimpleMath::Color& colour)
        {
            _render_target->clear(_context, colour);
        }

        const ComPtr<ID3D11Device>& Device::device() const
        {
            return _device;
        }

        const ComPtr<ID3D11DeviceContext>& Device::context() const
        {
            return _context;
        }

        void Device::present()
        {
            _swap_chain->Present(1, 0);
        }

        void Device::resize()
        {
            _context->ClearState();
            _render_target.reset();

            _swap_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
            create_render_target();
        }

        // Create the render target view from the swap chain that has been created.
        void Device::create_render_target()
        {
            Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer;
            _swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(back_buffer.GetAddressOf()));
            _render_target = std::make_unique<graphics::RenderTarget>(_device, back_buffer, graphics::RenderTarget::DepthStencilMode::Enabled);
        }
    }
}
