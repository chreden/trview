#pragma once

#include "../../UI/Fonts/IFonts.h"

namespace trview
{
    namespace mocks
    {
        struct MockFonts : public IFonts
        {
            MockFonts();
            virtual ~MockFonts();
            MOCK_METHOD(std::vector<FontSetting>, list_fonts, (), (const, override));
            MOCK_METHOD(ImFont*, add_font, (const std::string&, const FontSetting&), (override));
            MOCK_METHOD(ImFont*, font, (const std::string&), (const, override));
        };
    }
}
