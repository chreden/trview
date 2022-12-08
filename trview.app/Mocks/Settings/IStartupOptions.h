#pragma once

#include "../../Settings/IStartupOptions.h"

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
