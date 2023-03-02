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
        };
    }
}