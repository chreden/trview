#pragma once

#include "../../Windows/Console/IConsoleManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockConsoleManager : public IConsoleManager
        {
            MockConsoleManager();
            virtual ~MockConsoleManager();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(std::weak_ptr<IConsole>, create_window, (), (override));
            MOCK_METHOD(void, initialise_ui, (), (override));
            MOCK_METHOD(void, print, (const std::string&), (override));
        };
    }
}