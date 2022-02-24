#pragma once

#include "../../Tools/ICompass.h"

namespace trview
{
    namespace mocks
    {
        struct MockCompass : public ICompass
        {
            virtual ~MockCompass() = default;
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&), (override));
            MOCK_METHOD(bool, pick, (const Point&, const Size&, Axis&), (override));
            MOCK_METHOD(void, set_visible, (bool), (override));
        };
    }
}
