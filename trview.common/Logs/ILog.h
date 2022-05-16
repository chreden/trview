#pragma once

#include "Message.h"

namespace trview
{
    struct ILog
    {
        virtual ~ILog() = 0;
        virtual void log(const std::string& topic, const std::string& activity, const std::string& text) = 0;
        virtual void log(const std::string& topic, const std::vector<std::string>& activity, const std::string& text) = 0;
        virtual std::vector<Message> messages() const = 0;
        virtual std::vector<Message> messages(const std::string& topic, const std::string& activity) const = 0;
        virtual std::vector<std::string> topics() const = 0;
        virtual std::vector<std::string> activities(const std::string& topic) const = 0;
    };
}
