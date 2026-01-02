#include "Messages.h"

namespace trview
{
    namespace tests
    {
        std::optional<trview::Message> find_message(const std::vector<trview::Message>& messages, const std::string& type)
        {
            for (const auto& message : messages)
            {
                if (message.type == type)
                {
                    return message;
                }
            }
            return std::nullopt;
        }
    }
}