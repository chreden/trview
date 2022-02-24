#pragma once

#include "../../Settings/IStartupOptions.h"

namespace trview
{
    namespace mocks
    {
        struct MockStartupOptions : public IStartupOptions
        {
            virtual ~MockStartupOptions() = default;
            MOCK_METHOD(std::string, filename, (), (const, override));
            MOCK_METHOD(bool, feature, (const std::string&), (const, override));
        };
    }
}
