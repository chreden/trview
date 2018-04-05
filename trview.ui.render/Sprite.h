#pragma once

#include <atlbase.h>
#include <d3d11.h>
#include <cstdint>

#include <external/DirectXTK/Inc/SimpleMath.h>

namespace trview
{
    namespace ui
    {
        namespace render
        {
            class Sprite
            {
            public:
                Sprite(CComPtr<ID3D11Device> device, uint32_t width, uint32_t height);

                void render(CComPtr<ID3D11DeviceContext> context, CComPtr<ID3D11ShaderResourceView> texture, float x, float y, float width, float height, DirectX::SimpleMath::Color colour = { 1,1,1,1 });

                void set_host_size(uint32_t width, uint32_t height);

                Sprite(const Sprite&) = delete;
                Sprite& operator=(const Sprite&) = delete;
            private:
                void create_matrix();

                void update_matrix(CComPtr<ID3D11DeviceContext> context, float x, float y, float width, float height, const DirectX::SimpleMath::Color& colour);

                CComPtr<ID3D11Device>       _device;
                CComPtr<ID3D11VertexShader> _vertex_shader;
                CComPtr<ID3D11PixelShader>  _pixel_shader;
                CComPtr<ID3D11InputLayout>  _input_layout;
                CComPtr<ID3D11Buffer>       _vertex_buffer;
                CComPtr<ID3D11Buffer>       _index_buffer;
                CComPtr<ID3D11SamplerState> _sampler_state;
                CComPtr<ID3D11Buffer>       _matrix_buffer;

                uint32_t _host_width;
                uint32_t _host_height;
            };
        }
    }
}
