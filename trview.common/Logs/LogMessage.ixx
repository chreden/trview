export module trview.common:LogMessage;

import std;

namespace trview
{
    export struct LogMessage
    {
        enum class Status
        {
            Information,
            Warning,
            Error
        };

        Status status;
        std::string timestamp;
        std::string topic;
        std::vector<std::string> activity;
        std::string text;
    };
}
