#pragma once

#include "ILog.h"
#include "LogMessage.h"
#include <mutex>

namespace trview
{
    class Log final : public ILog
    {
    public:
        virtual ~Log() = default;
        virtual void log(LogMessage::Status status, const std::string& topic, const std::string& activity, const std::string& text) override;
        virtual void log(LogMessage::Status status, const std::string& topic, const std::vector<std::string>& activity, const std::string& text) override;
        virtual std::vector<LogMessage> messages() const override;
        virtual std::vector<LogMessage> messages(const std::string& topic, const std::string& activity) const override;
        virtual std::vector<std::string> topics() const override;
        virtual std::vector<std::string> activities(const std::string& topic) const override;
        virtual void clear() override;
    private:
        std::vector<LogMessage> _messages;
        mutable std::mutex _mutex;
    };
}
