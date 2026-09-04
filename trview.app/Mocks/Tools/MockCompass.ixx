module;

#include <gmock/gmock.h>

export module trview.app:MockCompass;

import :ICompass;

namespace trview
{
    namespace mocks
    {
        export struct MockCompass : public ICompass
        {
            MockCompass();
            virtual ~MockCompass();
            MOCK_METHOD(void, render, (const ICamera&), (override));
            MOCK_METHOD(bool, pick, (const Point&, const Size&, Axis&), (override));
            MOCK_METHOD(void, set_visible, (bool), (override));
        };
    }
}
