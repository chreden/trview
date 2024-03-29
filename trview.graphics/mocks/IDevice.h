#pragma once

#include "../IDevice.h"
#include "../DeviceWindow.h"

namespace trview
{
    namespace graphics
    {
        namespace mocks
        {
            struct MockDevice : public IDevice
            {
                MockDevice();
                virtual ~MockDevice();
                MOCK_METHOD(void, begin, (), (override));
                MOCK_METHOD(Microsoft::WRL::ComPtr<ID3D11Device>, device, (), (const, override));
                MOCK_METHOD(Microsoft::WRL::ComPtr<ID3D11DeviceContext>, context, (), (const, override));
                MOCK_METHOD(Microsoft::WRL::ComPtr<ID3D11BlendState>, create_blend_state, (const D3D11_BLEND_DESC&), (const, override));
                MOCK_METHOD(Microsoft::WRL::ComPtr<ID3D11Buffer>, create_buffer, (const D3D11_BUFFER_DESC&, const std::optional<D3D11_SUBRESOURCE_DATA>&), (const, override));
                MOCK_METHOD(Microsoft::WRL::ComPtr<ID3D11DepthStencilState>, create_depth_stencil_state, (const D3D11_DEPTH_STENCIL_DESC&), (const, override));
                MOCK_METHOD(Microsoft::WRL::ComPtr<ID3D11DepthStencilView>, create_depth_stencil_view, (const Microsoft::WRL::ComPtr<ID3D11Resource>&, const D3D11_DEPTH_STENCIL_VIEW_DESC&), (const, override));
                MOCK_METHOD(Microsoft::WRL::ComPtr<ID3D11RasterizerState>, create_rasterizer_state, (const D3D11_RASTERIZER_DESC&), (const, override));
                MOCK_METHOD(Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, create_render_target_view, (const Microsoft::WRL::ComPtr<ID3D11Resource>&), (const, override));
                MOCK_METHOD(Microsoft::WRL::ComPtr<ID3D11SamplerState>, create_sampler_state, (const D3D11_SAMPLER_DESC&), (const, override));
                MOCK_METHOD(Microsoft::WRL::ComPtr<ID3D11Texture2D>, create_texture_2D, (const D3D11_TEXTURE2D_DESC&, const D3D11_SUBRESOURCE_DATA&), (const, override));
                MOCK_METHOD(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>, create_shader_resource_view, (const Microsoft::WRL::ComPtr<ID3D11Texture2D>&), (const, override));
            };
        }
    }
}