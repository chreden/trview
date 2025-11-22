#include "Messages.h"

#include <trview.common/Messages/IMessageSystem.h>

#include "../Settings/UserSettings.h"

namespace trview
{
    namespace messages
    {
        namespace
        {
            void get_message(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to, const std::string& name)
            {
                if (auto ms = messaging.lock())
                {
                    ms->send_message(Message{ .type = name, .data = std::make_shared<MessageData<std::weak_ptr<IRecipient>>>(reply_to) });
                }
            }

            template <typename T>
            std::optional<T> read_message(const Message& message, const std::string& type)
            {
                if (message.type != type)
                {
                    return std::nullopt;
                }
                return std::static_pointer_cast<MessageData<T>>(message.data)->value;
            }

            template <typename T>
            void send_message(const std::weak_ptr<IMessageSystem>& messaging, const T& value, const std::string& type)
            {
                if (auto ms = messaging.lock())
                {
                    ms->send_message({ .type = type, .data = std::make_shared<MessageData<T>>(value) });
                }
            }
        }

        void get_settings(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to)
        {
            get_message(messaging, reply_to, "get_settings");
        }

        std::optional<UserSettings> read_settings(const Message& message)
        {
            return read_message<UserSettings>(message, "settings");
        }

        void send_settings(const std::weak_ptr<IMessageSystem>& messaging, const UserSettings& settings)
        {
            send_message(messaging, settings, "settings");
        }

        void get_selected_item(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to)
        {
            get_message(messaging, reply_to, "get_selected_item");
        }

        std::optional<std::weak_ptr<IItem>> read_select_item(const Message& message)
        {
            return read_message<std::weak_ptr<IItem>>(message, "select_item");
        }

        void send_select_item(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IItem>& item)
        {
            send_message(messaging, item, "select_item");
        }

        void get_selected_room(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to)
        {
            get_message(messaging, reply_to, "get_selected_room");
        }

        std::optional<std::weak_ptr<IRoom>> read_select_room(const Message& message)
        {
            return read_message<std::weak_ptr<IRoom>>(message, "select_room");
        }

        void send_select_room(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRoom>& room)
        {
            send_message(messaging, room, "select_room");
        }

        void get_selected_light(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to)
        {
            get_message(messaging, reply_to, "get_selected_light");
        }

        std::optional<std::weak_ptr<ILight>> read_select_light(const Message& message)
        {
            return read_message<std::weak_ptr<ILight>>(message, "select_light");
        }

        void send_select_light(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ILight>& light)
        {
            send_message(messaging, light, "select_light");
        }
    }
}
