/// @file Font.h
/// @brief Allows for rendering text of a specified style and size onto a texture.
/// 
/// Handles creation and rendering of fonts and associated textures. Also provides the ability
/// to measure a string for layout purposes. Uses DirectXTK SpriteFont to render the text.

#pragma once

import trview.common.size;

#include <string>
#include <wrl/client.h>
#include <memory>
#include <external/DirectXTK/Inc/SpriteFont.h>
#include <external/DirectXTK/Inc/SpriteBatch.h>

#include "TextAlignment.h"
#include "ParagraphAlignment.h"
#include "IFont.h"

namespace trview
{
    struct Colour;

    namespace graphics
    {
        class Texture;

        /// Creates, stores and renders fonts to textures.
        class Font final : public IFont
        {
        public:
            /// Create a new font.
            /// @param font The font face to use to render.
            /// @param text_alignment Text alignment for the font.
            /// @param paragraph_alignment Paragraph alignment for the font.
            explicit Font(const std::shared_ptr<DirectX::SpriteFont>& font, TextAlignment text_alignment, ParagraphAlignment paragraph_alignment);

            virtual ~Font() = default;

            /// Renders the text to the specified font texture.
            /// @param text The text to render on to the font texture.
            /// @param width The optional width of the rectangle in which to render text.
            /// @param height The optional height of the rectangle in which to render text.
            /// @param colour The colour to render the text.
            virtual void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const std::wstring& text, float width, float height, const Colour& colour) override;

            /// Renders the text to the specified font texture.
            /// @param text The text to render on to the font texture.
            /// @param x The x position to render the text.
            /// @param y The y position to render the text.
            /// @param width The optional width of the rectangle in which to render text.
            /// @param height The optional height of the rectangle in which to render text.
            /// @param colour The colour to render the text.
            virtual void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const std::wstring& text, float x, float y, float width, float height, const Colour& colour) override;

            /// Determines the size in pixels that the text specified will be when rendered.
            /// @param text The text to measure.
            /// @returns The size in pixels required to render the specified text.
            virtual Size measure(const std::wstring& text) const override;

            /// Determines whether the character is in the image set.
            /// @param character The character to test.
            /// @returns True if the character is in the image set.
            virtual bool is_valid_character(wchar_t character) const override;
        private:
            std::shared_ptr<DirectX::SpriteFont> _font;
            std::unique_ptr<DirectX::SpriteBatch> _batch;
            TextAlignment                        _text_alignment;
            ParagraphAlignment                   _paragraph_alignment;
        };
    }
}
