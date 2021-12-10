#pragma once

#include "../IFontMeasurer.h"

namespace trview
{
    namespace ui
    {
        namespace mocks
        {
            struct MockFontMeasurer : public IFontMeasurer
            {
                virtual ~MockFontMeasurer() = default;
                MOCK_METHOD(Size, measure, (const std::wstring&), (const, override));
                MOCK_METHOD(bool, is_valid_character, (wchar_t), (const, override));
            };
        }
    }
}
