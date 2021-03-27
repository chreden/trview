#pragma once

#include "../../Tools/ICompass.h"

namespace trview
{
    class MockCompass final : public ICompass
    {
        MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&));
        MOCK_METHOD(bool, pick, (const Point&, const Size&, Axis&));
        MOCK_METHOD(void, set_visible, (bool));
    };
}
