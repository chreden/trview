#pragma once

#include "../../Logs/ILog.h"

namespace trview
{
    namespace mocks
    {
        struct MockLog : public ILog
        {
            virtual ~MockLog() = default;
            MOCK_METHOD(void, log, (const std::string&, const std::string&, const std::string&), (override));
            MOCK_METHOD(void, log, (const std::string&, const std::vector<std::string>&, const std::string&), (override));
            MOCK_METHOD(std::vector<Message>, messages, (), (const, override));
        };
    }
}
