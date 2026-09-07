export module trview.tests.common:Messages;

import std;
import trview.common;

namespace trview
{
    namespace tests
    {
        export std::optional<trview::Message> find_message(const std::vector<trview::Message>& messages, const std::string& type);
        export trview::Message find_message_throw(const std::vector<trview::Message>& messages, const std::string& type);
        export std::optional<trview::Message> find_last_message(const std::vector<trview::Message>& messages, const std::string& type);
        export trview::Message find_last_message_throw(const std::vector<trview::Message>& messages, const std::string& type);
    }
}