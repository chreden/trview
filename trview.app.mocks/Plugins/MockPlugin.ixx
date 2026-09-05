module;

#include <gmock/gmock.h>

export module trview.app.mocks:MockPlugin;

import trview.app;

namespace trview
{
    namespace mocks
    {
        export struct MockPlugin : public IPlugin
        {
            MOCK_METHOD(bool, built_in, (), (const, override));
            MOCK_METHOD(std::string, name, (), (const, override));
            MOCK_METHOD(std::string, author, (), (const, override));
            MOCK_METHOD(std::string, description, (), (const, override));
            MOCK_METHOD(bool, enabled, (), (const, override));
            MOCK_METHOD(void, initialise, (IApplication*), (override));
            MOCK_METHOD(std::string, path, (), (const, override));
            MOCK_METHOD(std::string, messages, (), (const, override));
            MOCK_METHOD(void, execute, (const std::string&), (override));
            MOCK_METHOD(void, add_message, (const std::string&), (override));
            MOCK_METHOD(void, do_file, (const std::string&), (override));
            MOCK_METHOD(void, clear_messages, (), (override));
            MOCK_METHOD(void, reload, (), (override));
            MOCK_METHOD(void, render_toolbar, (), (override));
            MOCK_METHOD(void, render_ui, (), (override));
            MOCK_METHOD(void, set_enabled, (bool), (override));
        };
    }
}