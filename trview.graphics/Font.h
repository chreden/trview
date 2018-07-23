/// @file Font.h
/// @brief Allows for rendering text of a specified style and size onto a texture.
/// 
/// Handles creation and rendering of fonts and associated textures. Also provides the ability
/// to measure a string for layout purposes. Uses DirectWrite and Direct2D to render to the
/// D3D textures that are used in the rest of the program.

#pragma once

#include <string>
#include <wrl/client.h>

#include <external/DirectXTK/Inc/SpriteFont.h>
#include "TextAlignment.h"
#include "ParagraphAlignment.h"

namespace trview
{
    struct Size;
    struct Colour;

    namespace graphics
    {
        class Texture;

        /// Creates, stores and renders fonts to textures.
        class Font
        {
        public:
            /// Create a new font.
            /// @param size: The size of the font to use.
            /// @param text_alignment: Text alignment for the font.
            /// @param paragraph_alignment: Paragraph alignment for the font.
            explicit Font(const Microsoft::WRL::ComPtr<ID3D11Device>& device, float size, TextAlignment text_alignment, ParagraphAlignment paragraph_alignment);

            /// Renders the text to the specified font texture.
            /// @param text The text to render on to the font texture.
            /// @param x The x position on the texture at which to render the text.
            /// @param y The y position on the texture at which to render the text.
            /// @param width The optional width of the rectangle in which to render text. Defaults to 256.
            /// @param height The optional height of the rectangle in which to render text. Defaults to 256.
            void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const std::wstring& text, float x, float y, float width = 256, float height = 256);

            /// Determines the size in pixels that the text specified will be when rendered.
            /// @param text The text to measure.
            /// @returns The size in pixels required to render the specified text.
            Size measure(const std::wstring& text) const;
        private:
            DirectX::SpriteFont _font;
        };
    }
}
