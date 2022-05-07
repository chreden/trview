#pragma once

#include "ILog.h"
#include "Message.h"

namespace trview
{
    class Log final : public ILog
    {
    public:
        virtual ~Log() = default;
        virtual void log(const std::string& topic, const std::string& activity, const std::string& text) override;
        virtual std::vector<Message> messages() const override;
    private:
        std::vector<Message> _messages;
    };
}
