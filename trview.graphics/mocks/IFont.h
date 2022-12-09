#pragma once

#include "../IFont.h"

namespace trview
{
    namespace graphics
    {
        namespace mocks
        {
            class MockFont final : public graphics::IFont
            {
            public:
                MockFont();
                virtual ~MockFont();
                MOCK_METHOD(void, render, (const Microsoft::WRL::ComPtr<ID3D11DeviceContext>&, const std::wstring&, float, float, const Colour&));
                MOCK_METHOD(void, render, (const Microsoft::WRL::ComPtr<ID3D11DeviceContext>&, const std::wstring&, float, float, float, float, const Colour&));
                MOCK_METHOD(Size, measure, (const std::wstring&), (const));
                MOCK_METHOD(bool, is_valid_character, (wchar_t), (const));
            };
        }
    }
}