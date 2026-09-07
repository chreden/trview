module;

#include <gmock/gmock.h>

export module trview.app.mocks:MockWindow;

import trview.app;

namespace trview
{
    namespace mocks
    {
        export struct MockWindow : public IWindow
        {
            MOCK_METHOD(void, update, (float), (override));
            MOCK_METHOD(void, receive_message, (const Message&), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
            MOCK_METHOD(std::string, title, (), (const, override));
            MOCK_METHOD(std::string, type, (), (const, override));
        };
    }
}
