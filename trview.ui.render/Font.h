#pragma once

#include <string>
#include <atlbase.h>
#include <d2d1.h>
#include <dwrite.h>
#include <d3d11.h> 

#include "FontTexture.h"

namespace trview
{
    namespace ui
    {
        struct Size;

        namespace render
        {
            class Font
            {
            public:
                explicit Font(
                    CComPtr<ID3D11Device> device,
                    CComPtr<IDWriteFactory> dwrite_factory,
                    CComPtr<ID2D1Factory> d2d_factory,
                    CComPtr<IDWriteTextFormat> text_format);

                FontTexture create_texture(CComPtr<ID3D11Texture2D> texture);

                // Create a texture that can be used for font rendering.
                FontTexture create_texture();

                // Render the text to the specified texture.
                void render(FontTexture& texture, const std::wstring& text, float x, float y, float width = 256, float height = 256);

                // Determines the size in pixels that the text specified will be when rendered.
                // text: The text to measure.
                // Returns: The size in pixels required.
                Size measure(const std::wstring& text) const;
            private:
                CComPtr<ID3D11Device>      _device;
                CComPtr<IDWriteFactory>    _dwrite_factory;
                CComPtr<ID2D1Factory>      _d2d_factory;
                CComPtr<IDWriteTextFormat> _text_format;
            };
        }
    }
}
