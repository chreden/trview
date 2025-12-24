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

        std::optional<bool> read_ng_plus(const Message& message)
        {
            return read_message<bool>(message, "ng_plus");
        }

        void send_ng_plus(const std::weak_ptr<IMessageSystem>& messaging, bool value)
        {
            send_message(messaging, value, "ng_plus");
        }

        void get_open_level(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to)
        {
            get_message(messaging, reply_to, "get_open_level");
        }

        std::optional<std::weak_ptr<ILevel>> read_open_level(const Message& message)
        {
            return read_message<std::weak_ptr<ILevel>>(message, "open_level");
        }

        void send_open_level(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ILevel>& level)
        {
            send_message(messaging, level, "open_level");
        }

        std::optional<std::weak_ptr<ILevel>> read_end_diff(const Message& message)
        {
            return read_message<std::weak_ptr<ILevel>>(message, "end_diff");
        }

        void send_end_diff(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ILevel>& level)
        {
            send_message(messaging, level, "end_diff");
        }

        void get_selected_camera_sink(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to)
        {
            get_message(messaging, reply_to, "get_selected_camera_sink");
        }

        std::optional<std::weak_ptr<ICameraSink>> read_select_camera_sink(const Message& message)
        {
            return read_message<std::weak_ptr<ICameraSink>>(message, "select_camera_sink");
        }

        void send_select_camera_sink(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ICameraSink>& camera_sink)
        {
            send_message(messaging, camera_sink, "select_camera_sink");
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

        void get_selected_flyby_node(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to)
        {
            get_message(messaging, reply_to, "get_selected_flyby_node");
        }

        std::optional<std::weak_ptr<IFlybyNode>> read_select_flyby_node(const Message& message)
        {
            return read_message<std::weak_ptr<IFlybyNode>>(message, "select_flyby_node");
        }

        void send_select_flyby_node(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IFlybyNode>& flyby_node)
        {
            send_message(messaging, flyby_node, "select_flyby_node");
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

        void get_selected_sector(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to)
        {
            get_message(messaging, reply_to, "get_selected_sector");
        }

        std::optional<std::weak_ptr<ISector>> read_select_sector(const Message& message)
        {
            return read_message<std::weak_ptr<ISector>>(message, "select_sector");
        }

        void send_select_sector(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ISector>& sector)
        {
            send_message(messaging, sector, "select_sector");
        }

        void get_selected_sound_source(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to)
        {
            get_message(messaging, reply_to, "get_selected_sound_source");
        }

        std::optional<std::weak_ptr<ISoundSource>> read_select_sound_source(const Message& message)
        {
            return read_message<std::weak_ptr<ISoundSource>>(message, "select_sound_source");
        }

        void send_select_sound_source(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ISoundSource>& sound_source)
        {
            send_message(messaging, sound_source, "select_sound_source");
        }

        void get_selected_static_mesh(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to)
        {
            get_message(messaging, reply_to, "get_selected_static_mesh");
        }

        std::optional<std::weak_ptr<IStaticMesh>> read_select_static_mesh(const Message& message)
        {
            return read_message<std::weak_ptr<IStaticMesh>>(message, "select_static_mesh");
        }

        void send_select_static_mesh(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IStaticMesh>& static_mesh)
        {
            send_message(messaging, static_mesh, "select_static_mesh");
        }

        void get_selected_trigger(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to)
        {
            get_message(messaging, reply_to, "get_selected_trigger");
        }

        std::optional<std::weak_ptr<ITrigger>> read_select_trigger(const Message& message)
        {
            return read_message<std::weak_ptr<ITrigger>>(message, "select_trigger");
        }

        void send_select_trigger(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ITrigger>& trigger)
        {
            send_message(messaging, trigger, "select_trigger");
        }
    }
}
