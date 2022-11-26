#include "Device.h"
#include "RenderTarget.h"
#include "DeviceWindow.h"

using namespace Microsoft::WRL;

namespace trview
{
    namespace graphics
    {
        IDevice::~IDevice()
        {
        }

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

        ComPtr<ID3D11Device> Device::device() const
        {
            return _device;
        }

        ComPtr<ID3D11DeviceContext> Device::context() const
        {
            return _context;
        }

        ComPtr<ID3D11BlendState> Device::create_blend_state(const D3D11_BLEND_DESC& blend_desc) const
        {
            ComPtr<ID3D11BlendState> blend_state;
            _device->CreateBlendState(&blend_desc, &blend_state);
            return blend_state;
        }

        ComPtr<ID3D11Buffer> Device::create_buffer(const D3D11_BUFFER_DESC& buffer_desc, const std::optional<D3D11_SUBRESOURCE_DATA>& buffer_data) const
        {
            ComPtr<ID3D11Buffer> buffer;
            _device->CreateBuffer(&buffer_desc,
                buffer_data.has_value() ? &buffer_data.value() : nullptr,
                buffer.GetAddressOf());
            return buffer;
        }

        ComPtr<ID3D11DepthStencilState> Device::create_depth_stencil_state(const D3D11_DEPTH_STENCIL_DESC& depth_stencil_desc) const
        {
            ComPtr<ID3D11DepthStencilState> depth_stencil;
            _device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil);
            return depth_stencil;
        }

        ComPtr<ID3D11DepthStencilView> Device::create_depth_stencil_view(const Microsoft::WRL::ComPtr<ID3D11Resource>& resource, const D3D11_DEPTH_STENCIL_VIEW_DESC& depth_stencil_view_desc) const
        {
            ComPtr<ID3D11DepthStencilView> depth_stencil_view;
            _device->CreateDepthStencilView(resource.Get(), &depth_stencil_view_desc, &depth_stencil_view);
            return depth_stencil_view;
        }

        ComPtr<ID3D11RasterizerState> Device::create_rasterizer_state(const D3D11_RASTERIZER_DESC& rasterizer_desc) const
        {
            ComPtr<ID3D11RasterizerState> rasterizer_state;
            _device->CreateRasterizerState(&rasterizer_desc, &rasterizer_state);
            return rasterizer_state;
        }

        ComPtr<ID3D11RenderTargetView> Device::create_render_target_view(const Microsoft::WRL::ComPtr<ID3D11Resource>& resource) const
        {
            ComPtr<ID3D11RenderTargetView> render_target_view;
            _device->CreateRenderTargetView(resource.Get(), nullptr, &render_target_view);
            return render_target_view;
        }

        ComPtr<ID3D11SamplerState> Device::create_sampler_state(const D3D11_SAMPLER_DESC& sampler_desc) const
        {
            ComPtr<ID3D11SamplerState> sampler_state;
            _device->CreateSamplerState(&sampler_desc, &sampler_state);
            return sampler_state;
        }

        ComPtr<ID3D11Texture2D> Device::create_texture_2D(const D3D11_TEXTURE2D_DESC& texture_desc, const D3D11_SUBRESOURCE_DATA& texture_data) const
        {
            ComPtr<ID3D11Texture2D> texture;
            _device->CreateTexture2D(&texture_desc, &texture_data, &texture);
            return texture;
        }

        ComPtr<ID3D11ShaderResourceView> Device::create_shader_resource_view(const Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture) const
        {
            ComPtr<ID3D11ShaderResourceView> view;
            _device->CreateShaderResourceView(texture.Get(), nullptr, &view);
            return view;
        }
    }
}
