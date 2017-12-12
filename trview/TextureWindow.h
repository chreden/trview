#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <atlbase.h>
#include <cstdint>
#include <vector>

namespace trview
{
    class TextureWindow
    {
    public:
        explicit TextureWindow(CComPtr<ID3D11Device> device, uint32_t host_width, uint32_t host_height);
        void     set_textures(std::vector<CComPtr<ID3D11ShaderResourceView>> textures);
        void     render(CComPtr<ID3D11DeviceContext> context);
        void     cycle();

        // Set the size of the host window. This allows the texture window
        // to apply the correct scaling.
        void set_host_size(uint32_t width, uint32_t height);
    private:
        void create_scaling_matrix();

        CComPtr<ID3D11Device> _device;
        CComPtr<ID3D11VertexShader> _vertex_shader;
        CComPtr<ID3D11PixelShader> _pixel_shader;
        CComPtr<ID3D11InputLayout> _input_layout;
        CComPtr<ID3D11Buffer> _vertex_buffer;
        CComPtr<ID3D11Buffer> _index_buffer;
        CComPtr<ID3D11SamplerState> _sampler_state;
        CComPtr<ID3D11Buffer> _matrix_buffer;

        std::vector<CComPtr<ID3D11ShaderResourceView>> _level_textures;
        uint32_t _texture_index{ 0u };

        uint32_t _host_width;
        uint32_t _host_height;
    };
}