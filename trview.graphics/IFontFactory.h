#pragma once

#include <string>
#include <memory>
#include <external/DirectXTK/Inc/SpriteFont.h>

#include "TextAlignment.h"
#include "ParagraphAlignment.h"

namespace trview
{
    namespace graphics
    {
        struct IFont;

        struct IFontFactory
        {
            virtual ~IFontFactory() = 0;

            /// Store the given data in the font factory with the specfied key. The key should be 'fontX' - so 'Arial8' for example.
            virtual void store(const std::string& key, const std::shared_ptr<DirectX::SpriteFont>& font) = 0;

            /// Create a new font with the specified settings and alignment options.
            /// @param font_face The font face to use (eg Arial).
            /// @param size Size of the font.
            /// @param text_alignment Optional alignment of the text, describes how the text will be rendered
            ///        with respect to the layout rectangle in the horizontal axis. Defaults to TextAlignment::Left.
            /// @param paragraph_alignment Optional alignment of the text, describes how the text will be rendered
            ///        with respect to the layout rectangle in the vertical axis. Defaults to ParagraphAlignment::Near.
            /// @returns The new font instance.
            virtual std::unique_ptr<IFont> create_font(const std::string& font_face, int size, TextAlignment text_alignment = TextAlignment::Left, ParagraphAlignment paragraph_aligment = ParagraphAlignment::Near) const = 0;
        };
    }
}
