#include "Log.h"
#include <sstream>
#include <set>

namespace trview
{
    ILog::~ILog()
    {
    }

    void Log::log(const std::string& topic, const std::string& activity, const std::string& text)
    {
        log(topic, std::vector<std::string>{ activity }, text);
    }

    void Log::log(const std::string& topic, const std::vector<std::string>& activity, const std::string& text)
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

    std::vector<Message> Log::messages(const std::string& topic, const std::string& activity) const 
    {
        std::vector<Message> messages;
        std::copy_if(_messages.begin(), _messages.end(), std::back_inserter(messages),
            [&](const auto& m) { return m.topic == topic &&
                                        !m.activity.empty() &&
                                        m.activity[0] == activity; });
        return messages;
    }

    std::vector<std::string> Log::topics() const
    {
        std::set<std::string> all_topics;
        for (const auto& message : _messages)
        {
            all_topics.insert(message.topic);
        }
        return { all_topics.begin(), all_topics.end() };
    }

    std::vector<std::string> Log::activities(const std::string& topic) const
    {
        std::set<std::string> all_activities;
        for (const auto& message : _messages)
        {
            if (message.topic == topic && !message.activity.empty())
            {
                all_activities.insert(message.activity[0]);
            }
        }
        return { all_activities.begin(), all_activities.end() };
    }
}
