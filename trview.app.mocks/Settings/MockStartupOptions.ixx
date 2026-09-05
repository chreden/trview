module;

#include <gmock/gmock.h>

export module trview.app.mocks:MockStartupOptions;

import trview.app;

namespace trview
{
    namespace mocks
    {
        export struct MockStartupOptions : public IStartupOptions
        {
            MOCK_METHOD(std::string, filename, (), (const, override));
            MOCK_METHOD(bool, feature, (const std::string&), (const, override));
        };
    }
}
