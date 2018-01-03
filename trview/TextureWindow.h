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
#include "Texture.h"
#include "FontFactory.h"

namespace trview
{
    class TextureWindow
    {
    public:
        explicit TextureWindow(CComPtr<ID3D11Device> device, FontFactory& font_factory, uint32_t host_width, uint32_t host_height);
        void     set_textures(std::vector<Texture> textures);
        void     render(CComPtr<ID3D11DeviceContext> context);

        // Cycle through the textures in the level.
        void cycle();

        // Set the size of the host window. This allows the texture window
        // to apply the correct scaling.
        void set_host_size(uint32_t width, uint32_t height);

        void toggle_visibility();

        bool visible() const;
    private:
        void create_bg_texture();
        void render_text();

        CComPtr<ID3D11Device> _device;
        std::unique_ptr<Sprite> _sprite;

        std::vector<Texture> _level_textures;
        uint32_t _texture_index{ 0u };
        
        std::unique_ptr<Font> _font;

        // Background
        Texture _bg_texture;
        FontTexture _text_texture;
        CComPtr<ID3D11RenderTargetView> _render_target_view;
        bool _update_texture{ true };

        // Positioning.
        uint32_t _x { 10u };
        uint32_t _y { 10u };

        bool _visible{ true };
    };
}