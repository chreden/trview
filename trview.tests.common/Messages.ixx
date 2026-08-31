export module trview.tests.common:Messages;

import std;
import trview.common;

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