#include "Log.h"

namespace trview
{
    ILog::~ILog()
    {
    }

    void Log::log(const std::string& topic, const std::string& activity, const std::string& text)
    {
        _messages.push_back({ topic, activity, text });
    }

    std::vector<Message> Log::messages() const
    {
        return _messages;
    }
}
