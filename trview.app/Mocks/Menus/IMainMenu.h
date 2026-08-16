#pragma once

#include "../../Menus/IMainMenu.h"

namespace trview
{
    namespace mocks
    {
        struct MockMainMenu : public IMainMenu
        {
            MockMainMenu();
            virtual ~MockMainMenu();
            MOCK_METHOD(void, render, (), (override));
        };
    }
}
