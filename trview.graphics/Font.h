#pragma once

#include <string>
#include <wrl/client.h>
#include <d2d1.h>
#include <dwrite.h>
#include <d3d11.h> 

#include "FontTexture.h"

namespace trview
{
    struct Size;

    namespace graphics
    {
        class Texture;

        class Font
        {
        public:
            explicit Font(
                const Microsoft::WRL::ComPtr<ID3D11Device>& device,
                const Microsoft::WRL::ComPtr<IDWriteFactory>& dwrite_factory,
                const Microsoft::WRL::ComPtr<ID2D1Factory>& d2d_factory,
                const Microsoft::WRL::ComPtr<IDWriteTextFormat>& text_format);

            FontTexture create_texture(const graphics::Texture& texture);

            // Render the text to the specified texture.
            void render(FontTexture& texture, const std::wstring& text, float x, float y, float width = 256, float height = 256);

            // Determines the size in pixels that the text specified will be when rendered.
            // text: The text to measure.
            // Returns: The size in pixels required.
            Size measure(const std::wstring& text) const;
        private:
            Microsoft::WRL::ComPtr<ID3D11Device>      _device;
            Microsoft::WRL::ComPtr<IDWriteFactory>    _dwrite_factory;
            Microsoft::WRL::ComPtr<ID2D1Factory>      _d2d_factory;
            Microsoft::WRL::ComPtr<IDWriteTextFormat> _text_format;
        };
    }
}
