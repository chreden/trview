#pragma once

namespace trview
{
    struct Colour;
    struct Size;

    namespace graphics
    {
        struct IFont
        {
            virtual ~IFont() = 0;
            virtual void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const std::wstring& text, float width, float height, const Colour& colour) = 0;
            virtual void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const std::wstring& text, float x, float y, float width, float height, const Colour& colour) = 0;
            virtual Size measure(const std::wstring& text) const = 0;
            virtual  bool is_valid_character(wchar_t character) const = 0;
        };
    }
}
