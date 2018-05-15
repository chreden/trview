/// @file Font.h
/// @brief Allows for rendering text of a specified style and size onto a texture.
/// 
/// Handles creation and rendering of fonts and associated textures. Also provides the ability
/// to measure a string for layout purposes. Uses DirectWrite and Direct2D to render to the
/// D3D textures that are used in the rest of the program.

#pragma once

#include <string>
#include <wrl/client.h>
#include <d2d1.h>
#include <dwrite.h>

#include "FontTexture.h"

namespace trview
{
    struct Size;

    namespace graphics
    {
        class Texture;

        /// Creates, stores and renders fonts to textures.
        class Font
        {
        public:
            /// Create a new font.
            /// @param dwrite_factory: The DirectWrite factory to use to create DirectWrite resources.
            /// @param d2d_factory: The Direct2D factory to use to create Direct2D resources.
            /// @param text_format: The DirectWrite text format to use for the font.
            explicit Font(
                const Microsoft::WRL::ComPtr<IDWriteFactory>& dwrite_factory,
                const Microsoft::WRL::ComPtr<ID2D1Factory>& d2d_factory,
                const Microsoft::WRL::ComPtr<IDWriteTextFormat>& text_format);

            /// Create a texture that fonts can be rendered to based on the texture that is passed in.
            /// @param texture The texture that fonts will be rendered to.
            /// @returns A FontTexture that can be used with the render method.
            /// @see Font::render()
            /// @see FontTexture
            FontTexture create_texture(const graphics::Texture& texture);

            /// Renders the text to the specified font texture.
            /// @param texture The font texture to render to.
            /// @param text The text to render on to the font texture.
            /// @param x The x position on the texture at which to render the text.
            /// @param y The y position on the texture at which to render the text.
            /// @param width The optional width of the rectangle in which to render text. Defaults to 256.
            /// @param height The optional height of the rectangle in which to render text. Defaults to 256.
            /// @see create_texture
            /// @see FontTexture
            void render(FontTexture& texture, const std::wstring& text, float x, float y, float width = 256, float height = 256);

            /// Determines the size in pixels that the text specified will be when rendered.
            /// @param text The text to measure.
            /// @returns The size in pixels required to render the specified text.
            Size measure(const std::wstring& text) const;
        private:
            Microsoft::WRL::ComPtr<IDWriteFactory>    _dwrite_factory;
            Microsoft::WRL::ComPtr<ID2D1Factory>      _d2d_factory;
            Microsoft::WRL::ComPtr<IDWriteTextFormat> _text_format;
        };
    }
}
