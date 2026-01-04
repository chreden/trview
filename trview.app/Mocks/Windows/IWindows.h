#pragma once

#include "../../Windows/IWindows.h"

namespace trview
{
    namespace mocks
    {
        struct MockWindows : public IWindows
        {
            MockWindows();
            virtual ~MockWindows();

            MOCK_METHOD(std::weak_ptr<IWindow>, create, (const std::string&), (override));
            MOCK_METHOD(void, update, (float), (override));
            MOCK_METHOD(void, register_window, (const std::string&, const Creator&), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, setup, (const UserSettings&), (override));
            MOCK_METHOD(std::vector<std::weak_ptr<IWindow>>, windows, (const std::string&), (const, override));
        };
    }
}
