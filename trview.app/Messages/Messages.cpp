#include "Messages.h"

#include <trview.common/Messages/IMessageSystem.h>

#include "../Settings/UserSettings.h"

namespace trview
{
    namespace messages
    {
        void get_settings(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to)
        {
            if (auto ms = messaging.lock())
            {
                ms->send_message(Message{ .type = "get_settings", .data = std::make_shared<MessageData<std::weak_ptr<IRecipient>>>(reply_to) });
            }
        }

        std::optional<UserSettings> read_settings(const Message& message)
        {
            if (message.type != "settings")
            {
                return std::nullopt;
            }
            return std::static_pointer_cast<MessageData<UserSettings>>(message.data)->value;
        }

        void send_settings(const std::weak_ptr<IMessageSystem>& messaging, const UserSettings& settings)
        {
            if (auto ms = messaging.lock())
            {
                ms->send_message({ .type = "settings", .data = std::make_shared<MessageData<UserSettings>>(settings) });
            }
        }

        void get_selected_item(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to)
        {
            if (auto ms = messaging.lock())
            {
                ms->send_message(Message{ .type = "get_selected_item", .data = std::make_shared<MessageData<std::weak_ptr<IRecipient>>>(reply_to) });
            }
        }

        std::optional<std::weak_ptr<IItem>> read_select_item(const Message& message)
        {
            if (message.type != "select_item")
            {
                return std::nullopt;
            }
            return std::static_pointer_cast<MessageData<std::weak_ptr<IItem>>>(message.data)->value;
        }

        void send_select_item(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IItem>& item)
        {
            if (auto ms = messaging.lock())
            {
                ms->send_message({ .type = "select_item", .data = std::make_shared<MessageData<std::weak_ptr<IItem>>>(item) });
            }
        }

        void get_selected_room(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to)
        {
            if (auto ms = messaging.lock())
            {
                ms->send_message(Message{ .type = "get_selected_room", .data = std::make_shared<MessageData<std::weak_ptr<IRecipient>>>(reply_to) });
            }
        }

        std::optional<std::weak_ptr<IRoom>> read_select_room(const Message& message)
        {
            if (message.type != "select_room")
            {
                return std::nullopt;
            }
            return std::static_pointer_cast<MessageData<std::weak_ptr<IRoom>>>(message.data)->value;
        }

        void send_select_room(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRoom>& room)
        {
            if (auto ms = messaging.lock())
            {
                ms->send_message({ .type = "select_room", .data = std::make_shared<MessageData<std::weak_ptr<IRoom>>>(room) });
            }
        }
    }
}
