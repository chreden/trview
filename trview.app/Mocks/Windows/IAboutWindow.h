#pragma once

#include "../../Windows/About/IAboutWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockAboutWindow : public IAboutWindow
        {
            MockAboutWindow();
            virtual ~MockAboutWindow();
            MOCK_METHOD(void, focus, (), (override));
            MOCK_METHOD(void, render, (), (override));
        };
    }
}
