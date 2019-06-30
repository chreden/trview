#include "Device.h"
#include "RenderTarget.h"
#include "DeviceWindow.h"

using namespace Microsoft::WRL;

namespace trview
{
    namespace graphics
    {
        Device::Device()
        {
            D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                D3D11_SDK_VERSION, &_device, nullptr, &_context);

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

            D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
            memset(&depthStencilDesc, 0, sizeof(depthStencilDesc));
            depthStencilDesc.DepthEnable = true;
            depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
            depthStencilDesc.StencilEnable = true;
            depthStencilDesc.StencilReadMask = 0xFF;
            depthStencilDesc.StencilWriteMask = 0xFF;
            depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
            depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
            depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
            depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
            depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
            depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
            depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
            depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

            _device->CreateDepthStencilState(&depthStencilDesc, &_depth_stencil_state);
        }

        Device::~Device()
        {
        }

        void Device::begin()
        {
            _context->OMSetBlendState(_blend_state.Get(), 0, 0xffffffff);
            _context->OMSetDepthStencilState(_depth_stencil_state.Get(), 1);
        }

        const ComPtr<ID3D11Device>& Device::device() const
        {
            return _device;
        }

        const ComPtr<ID3D11DeviceContext>& Device::context() const
        {
            return _context;
        }

        std::unique_ptr<DeviceWindow> Device::create_for_window(const Window& window)
        {
            return std::make_unique<DeviceWindow>(*this, window);
        }
    }
}
