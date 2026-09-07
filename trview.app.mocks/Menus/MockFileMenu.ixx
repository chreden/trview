module;

#include <gmock/gmock.h>

export module trview.app.mocks:MockFileMenu;

import trview.app;

namespace trview
{
    namespace mocks
    {
        export struct MockFileMenu : public IFileMenu
        {
            MOCK_METHOD(std::vector<std::string>, local_levels, (), (const, override));
            MOCK_METHOD(void, open_file, (const std::string&, const std::weak_ptr<trlevel::IPack>&), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, switch_to, (const std::string&), (override));
        };
    }
}
