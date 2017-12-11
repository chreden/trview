#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <atlbase.h>

#include <vector>

namespace trview
{
    class TextureWindow
    {
    public:
        explicit TextureWindow(CComPtr<ID3D11Device> device);
        void     set_textures(std::vector<CComPtr<ID3D11ShaderResourceView>> textures);
        void     render(CComPtr<ID3D11DeviceContext> context);
        void     cycle();
    private:
        CComPtr<ID3D11VertexShader> _vertex_shader;
        CComPtr<ID3D11PixelShader> _pixel_shader;
        CComPtr<ID3D11InputLayout> _input_layout;
        CComPtr<ID3D11Buffer> _vertex_buffer;
        CComPtr<ID3D11Buffer> _index_buffer;
        CComPtr<ID3D11SamplerState> _sampler_state;

        std::vector<CComPtr<ID3D11ShaderResourceView>> _level_textures;
        uint32_t _texture_index{ 0u };
    };
}