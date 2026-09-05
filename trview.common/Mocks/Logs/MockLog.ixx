module;

#include <gmock/gmock.h>

export module trview.common:MockLog;

import :ILog;
import std;

namespace trview
{
    namespace mocks
    {
        export struct MockLog : public ILog
        {
            MOCK_METHOD(void, log, (LogMessage::Status, const std::string&, const std::string&, const std::string&), (override));
            MOCK_METHOD(void, log, (LogMessage::Status, const std::string&, const std::vector<std::string>&, const std::string&), (override));
            MOCK_METHOD(std::vector<LogMessage>, messages, (), (const, override));
            MOCK_METHOD(std::vector<LogMessage>, messages, (const std::string&, const std::string&), (const, override));
            MOCK_METHOD(std::vector<std::string>, topics, (), (const, override));
            MOCK_METHOD(std::vector<std::string>, activities, (const std::string&), (const, override));
            MOCK_METHOD(void, clear, (), (override));
        };
    }
}
