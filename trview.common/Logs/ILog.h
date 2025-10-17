#pragma once

#include "LogMessage.h"

namespace trview
{
    struct ILog
    {
        virtual ~ILog() = 0;
        virtual void log(LogMessage::Status status, const std::string& topic, const std::string& activity, const std::string& text) = 0;
        virtual void log(LogMessage::Status status, const std::string& topic, const std::vector<std::string>& activity, const std::string& text) = 0;
        virtual std::vector<LogMessage> messages() const = 0;
        virtual std::vector<LogMessage> messages(const std::string& topic, const std::string& activity) const = 0;
        virtual std::vector<std::string> topics() const = 0;
        virtual std::vector<std::string> activities(const std::string& topic) const = 0;
        virtual void clear() = 0;
    };
}
