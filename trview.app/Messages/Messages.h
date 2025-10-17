#pragma once

#include <optional>
#include <memory>

namespace trview
{
    struct Message;
    struct IRecipient;
    struct IMessageSystem;
    struct UserSettings;

    namespace messages
    {
        void get_settings(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        std::optional<UserSettings> read_settings(const Message& message);
        void send_settings(const std::weak_ptr<IMessageSystem>& messaging, const UserSettings& settings);
    }
}
