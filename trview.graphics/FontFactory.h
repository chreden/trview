#pragma once

#include <string>

#include <wrl/client.h>
#include <d2d1.h>
#include <dwrite.h>
#include <memory>

#include "Font.h"

namespace trview
{
    namespace graphics
    {
        class FontFactory
        {
        public:
            FontFactory();

            FontFactory(const FontFactory&) = delete;

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
