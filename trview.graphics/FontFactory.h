/// @file Font.h
/// @brief Creates fonts that can be used to render text to textures.
/// 
/// Handles creation of fonts using DirectWrite and Direct2D.

#pragma once

#include <string>

#include <wrl/client.h>
#include <d2d1.h>
#include <dwrite.h>
#include <memory>

namespace trview
{
    namespace graphics
    {
        class Font;

        /// Creates fonts that can be used to render text to textures.
        class FontFactory
        {
        public:
            /// Creates a new instace of the FontFactory class.
            FontFactory();

            ~FontFactory();

            FontFactory(const FontFactory&) = delete;

            /// Create a new font with the specified settings and alignment options.
            /// @param font_face The font face to use (eg Arial).
            /// @param size Optional size of the font. Defaults to 20.0f.
            /// @param text_alignment Optional alignment of the text, describes how the text will be rendered
            ///        with respect to the layout rectangle in the horizontal axis. Defaults to DWRITE_TEXT_ALIGNMENT_LEADING.
            /// @param paragraph_alignment Optional alignment of the text, describes how the text will be rendered
            ///        with respect to the layout rectangle in the vertical axis. Defaults to DWRITE_PARAGRAPH_ALIGNMENT_NEAR.
            /// @returns The new font instance.
            std::unique_ptr<Font> create_font(
                const std::wstring& font_face,
                float size = 20.f,
                DWRITE_TEXT_ALIGNMENT text_alignment = DWRITE_TEXT_ALIGNMENT_LEADING,
                DWRITE_PARAGRAPH_ALIGNMENT paragraph_alignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        private:
            Microsoft::WRL::ComPtr<ID2D1Factory>   _d2d_factory;
            Microsoft::WRL::ComPtr<IDWriteFactory> _dwrite_factory;
        };
    }
}
