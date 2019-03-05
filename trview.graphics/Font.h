/// @file Font.h
/// @brief Allows for rendering text of a specified style and size onto a texture.
/// 
/// Handles creation and rendering of fonts and associated textures. Also provides the ability
/// to measure a string for layout purposes. Uses DirectXTK SpriteFont to render the text.

#pragma once

#include <string>
#include <wrl/client.h>
#include <memory>
#include <external/DirectXTK/Inc/SpriteFont.h>
#include <external/DirectXTK/Inc/SpriteBatch.h>

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
            /// @param font The font face to use to render.
            /// @param text_alignment Text alignment for the font.
            /// @param paragraph_alignment Paragraph alignment for the font.
            explicit Font(const std::shared_ptr<DirectX::SpriteFont>& font, TextAlignment text_alignment, ParagraphAlignment paragraph_alignment);

            /// Renders the text to the specified font texture.
            /// @param text The text to render on to the font texture.
            /// @param width The optional width of the rectangle in which to render text.
            /// @param height The optional height of the rectangle in which to render text.
            /// @param colour The colour to render the text.
            void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const std::wstring& text, float width, float height, const Colour& colour);

            /// Determines the size in pixels that the text specified will be when rendered.
            /// @param text The text to measure.
            /// @returns The size in pixels required to render the specified text.
            Size measure(const std::wstring& text) const;

            /// Determines whether the character is in the image set.
            /// @param character The character to test.
            /// @returns True if the character is in the image set.
            bool is_valid_character(wchar_t character) const;
        private:
            std::shared_ptr<DirectX::SpriteFont> _font;
            std::unique_ptr<DirectX::SpriteBatch> _batch;
            TextAlignment                        _text_alignment;
            ParagraphAlignment                   _paragraph_alignment;
        };
    }
}
