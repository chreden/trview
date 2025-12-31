#pragma once

#include "../../Windows/IWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockWindow : public IWindow
        {
            MockWindow();
            virtual ~MockWindow();
            MOCK_METHOD(void, update, (float), (override));
            MOCK_METHOD(void, receive_message, (const Message&), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
            MOCK_METHOD(std::string, title, (), (const, override));
            MOCK_METHOD(std::string, type, (), (const, override));
        };
    }
}
