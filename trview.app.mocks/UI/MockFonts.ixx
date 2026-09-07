module;

#include <gmock/gmock.h>

export module trview.app.mocks:MockFonts;

import trview.app;

namespace trview
{
    namespace mocks
    {
        export struct MockFonts : public IFonts
        {
            MOCK_METHOD(std::vector<FontSetting>, list_fonts, (), (const, override));
            MOCK_METHOD(std::optional<Font>, add_font, (const std::string&, const FontSetting&), (override));
            MOCK_METHOD(std::optional<Font>, font, (const std::string&), (const, override));
            MOCK_METHOD((std::unordered_map<std::string, FontSetting>), fonts, (), (const, override));
        };
    }
}
