#include "Messages.h"
#include <format>
#include <ranges>

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

        trview::Message find_message_throw(const std::vector<trview::Message>& messages, const std::string& type)
        {
            for (const auto& message : messages)
            {
                if (message.type == type)
                {
                    return message;
                }
            }
            throw std::exception(std::format("Message of type '{}' not found", type).c_str());
        }

        std::optional<trview::Message> find_last_message(const std::vector<trview::Message>& messages, const std::string& type)
        {
            std::optional<trview::Message> result;
            for (const auto& message : messages)
            {
                if (message.type == type)
                {
                    result = message;
                }
            }
            return result;
        }

        trview::Message find_last_message_throw(const std::vector<trview::Message>& messages, const std::string& type)
        {
            for (const auto& message : messages | std::views::reverse)
            {
                if (message.type == type)
                {
                    return message;
                }
            }
            throw std::exception(std::format("Message of type '{}' not found", type).c_str());
        }
    }
}