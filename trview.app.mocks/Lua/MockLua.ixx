module;

#include <gmock/gmock.h>

export module trview.app.mocks:MockLua;

import trview.app;

namespace trview
{
    namespace mocks
    {
        export struct MockLua : public ILua
        {
            MockLua() {}
            ~MockLua() {}
            MOCK_METHOD(void, do_file, (const std::string&), (override));
            MOCK_METHOD(void, execute, (const std::string&), (override));
            MOCK_METHOD(void, initialise, (IApplication*), (override));
            MOCK_METHOD(void, set_directory, (const std::string&), (override));
        };
    }
}
