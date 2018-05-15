#include "FontFactory.h"

namespace trview
{
    namespace graphics
    {
        FontFactory::FontFactory()
        {
            D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, _d2d_factory.GetAddressOf());
            DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(_dwrite_factory.GetAddressOf()));
        }

        std::unique_ptr<Font> FontFactory::create_font(const std::wstring& font_face, float size, DWRITE_TEXT_ALIGNMENT text_alignment, DWRITE_PARAGRAPH_ALIGNMENT paragraph_alignment)
        {
            Microsoft::WRL::ComPtr<IDWriteTextFormat> text_format;
            _dwrite_factory->CreateTextFormat(
                font_face.c_str(),
                nullptr,
                DWRITE_FONT_WEIGHT_REGULAR,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                size,
                L"en-us",
                &text_format);

            text_format->SetTextAlignment(text_alignment);
            text_format->SetParagraphAlignment(paragraph_alignment);
            return std::make_unique<Font>(_dwrite_factory, _d2d_factory, text_format);
        }
    }
}
