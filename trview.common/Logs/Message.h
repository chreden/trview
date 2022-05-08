#pragma once

namespace trview
{
    struct Message
    {
        std::string timestamp;
        std::string topic;
        std::string activity;
        std::string text;
    };
}
