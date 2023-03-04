#pragma once

#include "../../Plugins/IPlugin.h"

namespace trview
{
    namespace mocks
    {
        struct MockPlugin : public IPlugin
        {
            MockPlugin();
            virtual ~MockPlugin();
            MOCK_METHOD(std::string, name, (), (const, override));
            MOCK_METHOD(std::string, author, (), (const, override));
            MOCK_METHOD(std::string, description, (), (const, override));
            MOCK_METHOD(void, initialise, (IApplication*), (override));
            MOCK_METHOD(std::string, path, (), (const, override));
            MOCK_METHOD(std::string, messages, (), (const, override));
            MOCK_METHOD(void, execute, (const std::string&), (override));
            MOCK_METHOD(void, add_message, (const std::string&), (override));
            MOCK_METHOD(void, do_file, (const std::string&), (override));
            MOCK_METHOD(void, clear_messages, (), (override));
        };
    }
}