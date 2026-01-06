#pragma once

#include <vector>
#include <optional>
#include <string>
#include <trview.common/Messages/Message.h>

namespace trview
{
    namespace tests
    {
        std::optional<trview::Message> find_message(const std::vector<trview::Message>& messages, const std::string& type);
        trview::Message find_message_throw(const std::vector<trview::Message>& messages, const std::string& type);
        std::optional<trview::Message> find_last_message(const std::vector<trview::Message>& messages, const std::string& type);
        trview::Message find_last_message_throw(const std::vector<trview::Message>& messages, const std::string& type);
    }
}