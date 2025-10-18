#pragma once

#include <optional>
#include <memory>

namespace trview
{
    struct IItem;
    struct IMessageSystem;
    struct IRecipient;
    struct Message;
    struct UserSettings;

    namespace messages
    {
        void get_settings(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        std::optional<UserSettings> read_settings(const Message& message);
        void send_settings(const std::weak_ptr<IMessageSystem>& messaging, const UserSettings& settings);

        void get_selected_item(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        std::optional<std::weak_ptr<IItem>> read_select_item(const Message& message);
        void send_select_item(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IItem>& item);
    }
}
