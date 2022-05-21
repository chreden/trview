#pragma once

#include "../../Menus/IFileMenu.h"

namespace trview
{
    namespace mocks
    {
        struct MockFileMenu : public IFileMenu
        {
            virtual ~MockFileMenu() = default;
            MOCK_METHOD(void, open_file, (const std::string&), (override));
            MOCK_METHOD(void, set_recent_files, (const std::list<std::string>&), (override));
        };
    }
}
