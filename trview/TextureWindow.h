#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <atlbase.h>
#include <cstdint>
#include <vector>
#include <memory>

// Direct write and d2d
#include <dwrite.h>
#include <d2d1.h>

#include "Sprite.h"

namespace trview
{
    class TextureWindow
    {
    public:
        explicit TextureWindow(CComPtr<ID3D11Device> device, uint32_t host_width, uint32_t host_height);
        void     set_textures(std::vector<CComPtr<ID3D11ShaderResourceView>> textures);
        void     render(CComPtr<ID3D11DeviceContext> context);

        // Cycle through the textures in the level.
        void cycle();

        // Set the size of the host window. This allows the texture window
        // to apply the correct scaling.
        void set_host_size(uint32_t width, uint32_t height);
    private:

        void create_bg_texture();
        void initialise_d2d();

        void render_text();

        CComPtr<ID3D11Device> _device;
        std::unique_ptr<Sprite> _sprite;

        std::vector<CComPtr<ID3D11ShaderResourceView>> _level_textures;
        uint32_t _texture_index{ 0u };

        // Text rendering:
        CComPtr<IDWriteFactory> _dwrite_factory;
        CComPtr<IDWriteTextFormat> _text_format;
        CComPtr<ID2D1Factory> _d2d_factory;
        CComPtr<ID2D1RenderTarget> _d2d_rt;
        CComPtr<ID2D1SolidColorBrush> _d2d_brush;
        CComPtr<ID3D11Texture2D> _text_texture;
        CComPtr<ID3D11ShaderResourceView> _text_resource;

        // Background
        CComPtr<ID3D11Texture2D> _bg_texture;
        CComPtr<ID3D11ShaderResourceView> _bg_resource;

        // Positioning.
        uint32_t _x { 10u };
        uint32_t _y { 10u };
    };
}