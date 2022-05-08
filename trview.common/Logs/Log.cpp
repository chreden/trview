#include "Log.h"
#include <sstream>

namespace trview
{
    ILog::~ILog()
    {
    }

    void Log::log(const std::string& topic, const std::string& activity, const std::string& text)
    {
        // TODO: Make this nicer
        SYSTEMTIME time;
        GetLocalTime(&time);
        std::stringstream stream;
        stream << time.wDay << "-" << time.wMonth << "-" << time.wYear << " " << time.wHour << ":" << time.wMinute << ":" << time.wSecond;

        _messages.push_back({ stream.str(), topic, activity, text });
    }

    std::vector<Message> Log::messages() const
    {
        return _messages;
    }
}
