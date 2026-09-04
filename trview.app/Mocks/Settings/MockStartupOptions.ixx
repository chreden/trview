module;

#include <gmock/gmock.h>

export module trview.app:MockStartupOptions;

import :IStartupOptions;

namespace trview
{
    namespace mocks
    {
        struct MockStartupOptions : public IStartupOptions
        {
            MockStartupOptions();
            virtual ~MockStartupOptions();
            MOCK_METHOD(std::string, filename, (), (const, override));
            MOCK_METHOD(bool, feature, (const std::string&), (const, override));
        };
    }
}
