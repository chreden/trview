#pragma once

namespace trview
{
    struct ILog;

    class Activity
    {
    public:
        explicit Activity(const std::shared_ptr<ILog>& log, const std::string& topic, const std::string& name);
        void log(const std::string& text);
    private:
        std::shared_ptr<ILog> _log;
        std::string _topic;
        std::string _name;
    };
}
