#include "Log.h"
#include <set>
#include <format>

namespace trview
{
    ILog::~ILog()
    {
    }

    void Log::log(LogMessage::Status status, const std::string& topic, const std::string& activity, const std::string& text)
    {
        log(status, topic, std::vector<std::string>{ activity }, text);
    }

    void Log::log(LogMessage::Status status, const std::string& topic, const std::vector<std::string>& activity, const std::string& text)
    {
        SYSTEMTIME time;
        GetLocalTime(&time);
        std::lock_guard lock{ _mutex };
        _messages.push_back({
            status,
            std::format("{:02d}-{:02d}-{} {:02d}:{:02d}:{:02d}", time.wDay, time.wMonth, time.wYear, time.wHour, time.wMinute, time.wSecond),
            topic, activity, text });
    }

    std::vector<LogMessage> Log::messages() const
    {
        std::lock_guard lock{ _mutex };
        return _messages;
    }

    std::vector<LogMessage> Log::messages(const std::string& topic, const std::string& activity) const
    {
        std::vector<LogMessage> messages;
        {
            std::lock_guard lock{ _mutex };
            std::copy_if(_messages.begin(), _messages.end(), std::back_inserter(messages),
                [&](const auto& m) { return m.topic == topic &&
                !m.activity.empty() &&
                m.activity[0] == activity; });
        }
        return messages;
    }

    std::vector<std::string> Log::topics() const
    {
        std::set<std::string> all_topics;
        std::lock_guard lock{ _mutex };
        for (const auto& message : _messages)
        {
            all_topics.insert(message.topic);
        }
        return { all_topics.begin(), all_topics.end() };
    }

    std::vector<std::string> Log::activities(const std::string& topic) const
    {
        std::set<std::string> all_activities;
        std::lock_guard lock{ _mutex };
        for (const auto& message : _messages)
        {
            if (message.topic == topic && !message.activity.empty())
            {
                all_activities.insert(message.activity[0]);
            }
        }
        return { all_activities.begin(), all_activities.end() };
    }

    void Log::clear()
    {
        std::lock_guard lock{ _mutex };
        _messages.clear();
    }
}
