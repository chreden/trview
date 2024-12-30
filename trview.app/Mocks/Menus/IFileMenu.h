#pragma once

#include "../../Menus/IFileMenu.h"

namespace trview
{
    namespace mocks
    {
        struct MockFileMenu : public IFileMenu
        {
            MockFileMenu();
            virtual ~MockFileMenu();
            MOCK_METHOD(std::vector<std::string>, local_levels, (), (const, override));
            MOCK_METHOD(void, open_file, (const std::string&), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_recent_files, (const std::list<std::string>&), (override));
            MOCK_METHOD(void, switch_to, (const std::string&), (override));
        };
    }
}
