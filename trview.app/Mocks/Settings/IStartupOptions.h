#pragma once

#include "../../Settings/IStartupOptions.h"

namespace trview
{
    namespace mocks
    {
        class MockStartupOptions final : public IStartupOptions
        {
        public:
            virtual ~MockStartupOptions() = default;
            MOCK_METHOD(std::string, filename, (), (const));
            MOCK_METHOD(bool, feature, (const std::string&), (const));
        };
    }
}
