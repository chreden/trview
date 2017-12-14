#include "stdafx.h"
#include "FontFactory.h"
#include "Texture.h"

namespace trview
{
    FontFactory::FontFactory()
    {
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &_d2d_factory);
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&_dwrite_factory));
    }

    std::unique_ptr<Font> FontFactory::create_font(CComPtr<ID3D11Device> device, const std::wstring& font_face)
    {
        CComPtr<IDWriteTextFormat> text_format;
        _dwrite_factory->CreateTextFormat(
            font_face.c_str(),
            nullptr,
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            20.0f,
            L"en-us",
            &text_format);

        text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        return std::make_unique<Font>(device, _d2d_factory, text_format);
    }
}
