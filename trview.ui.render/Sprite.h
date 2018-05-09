#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <cstdint>

#include <SimpleMath.h>

namespace trview
{
    namespace graphics
    {
        struct IShader;
        struct IShaderStorage;
    }

    namespace ui
    {
        namespace render
        {
            class Sprite
            {
            public:
                Sprite(const Microsoft::WRL::ComPtr<ID3D11Device>& device, const graphics::IShaderStorage& shader_storage, const uint32_t width, uint32_t height);

                void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& texture, float x, float y, float width, float height, DirectX::SimpleMath::Color colour = { 1,1,1,1 });

                void set_host_size(uint32_t width, uint32_t height);

                Sprite(const Sprite&) = delete;
                Sprite& operator=(const Sprite&) = delete;
            private:
                void create_matrix(const Microsoft::WRL::ComPtr<ID3D11Device>& device);

                void update_matrix(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, float x, float y, float width, float height, const DirectX::SimpleMath::Color& colour);

                Microsoft::WRL::ComPtr<ID3D11Buffer>       _vertex_buffer;
                Microsoft::WRL::ComPtr<ID3D11Buffer>       _index_buffer;
                Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler_state;
                Microsoft::WRL::ComPtr<ID3D11Buffer>       _matrix_buffer;
                graphics::IShader*          _vertex_shader;
                graphics::IShader*          _pixel_shader;
                uint32_t                    _host_width;
                uint32_t                    _host_height;
            };
        }
    }
}
