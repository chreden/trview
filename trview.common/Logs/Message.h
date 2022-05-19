#pragma once

namespace trview
{
    struct Message
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
