/// @file FontFactory.h
/// @brief Creates fonts that can be used to render text to textures.
/// 
/// Handles creation of fonts using DirectWrite and Direct2D.

#pragma once

#include <string>
#include <wrl/client.h>
#include <memory>
#include <d3d11.h>

#include "TextAlignment.h"
#include "ParagraphAlignment.h"

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
            explicit FontFactory(const Microsoft::WRL::ComPtr<ID3D11Device>& device);

            ~FontFactory();

            FontFactory(const FontFactory&) = delete;

            /// Create a new font with the specified settings and alignment options.
            /// @param font_face The font face to use (eg Arial).
            /// @param size Optional size of the font. Defaults to 20.0f.
            /// @param text_alignment Optional alignment of the text, describes how the text will be rendered
            ///        with respect to the layout rectangle in the horizontal axis. Defaults to TextAlignment::Left.
            /// @param paragraph_alignment Optional alignment of the text, describes how the text will be rendered
            ///        with respect to the layout rectangle in the vertical axis. Defaults to ParagraphAlignment::Near.
            /// @returns The new font instance.
            std::unique_ptr<Font> create_font(
                const std::wstring& font_face,
                float size = 20.f,
                TextAlignment text_alignment = TextAlignment::Left,
                ParagraphAlignment paragraph_alignment = ParagraphAlignment::Near);
        private:
            Microsoft::WRL::ComPtr<ID3D11Device> _device;
        };
    }
}
