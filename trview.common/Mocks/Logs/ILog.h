#pragma once

#include "../../Logs/ILog.h"

namespace trview
{
    namespace mocks
    {
        struct MockLog : public ILog
        {
            virtual ~MockLog() = default;
            MOCK_METHOD(void, log, (Message::Status, const std::string&, const std::string&, const std::string&), (override));
            MOCK_METHOD(void, log, (Message::Status, const std::string&, const std::vector<std::string>&, const std::string&), (override));
            MOCK_METHOD(std::vector<Message>, messages, (), (const, override));
            MOCK_METHOD(std::vector<Message>, messages, (const std::string&, const std::string&), (const, override));
            MOCK_METHOD(std::vector<std::string>, topics, (), (const, override));
            MOCK_METHOD(std::vector<std::string>, activities, (const std::string&), (const, override));
            MOCK_METHOD(void, clear, (), (override));
        };
    }
}
