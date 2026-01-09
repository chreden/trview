#include "Messages.h"
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

        namespace commands
        {
            std::optional<bool> read_route_open(const Message& message)
            {
                return read_message<bool>(message, "route_open");
            }

            void send_route_open(const std::weak_ptr<IMessageSystem>& messaging)
            {
                send_message(messaging, true, "route_open");
            }

            std::optional<bool> read_route_reload(const Message& message)
            {
                return read_message<bool>(message, "route_reload");
            }

            void send_route_reload(const std::weak_ptr<IMessageSystem>& messaging)
            {
                send_message(messaging, true, "route_reload");
            }

            std::optional<bool> read_route_save(const Message& message)
            {
                return read_message<bool>(message, "route_save");
            }

            void send_route_save(const std::weak_ptr<IMessageSystem>& messaging)
            {
                send_message(messaging, true, "route_save");
            }

            std::optional<bool> read_route_save_as(const Message& message)
            {
                return read_message<bool>(message, "route_save_as");
            }

            void send_route_save_as(const std::weak_ptr<IMessageSystem>& messaging)
            {
                send_message(messaging, true, "route_save_as");
            }

            std::optional<bool> read_new_route(const Message& message)
            {
                return read_message<bool>(message, "new_route");
            }

            void send_new_route(const std::weak_ptr<IMessageSystem>& messaging)
            {
                send_message(messaging, true, "new_route");
            }

            std::optional<bool> read_new_randomizer_route(const Message& message)
            {
                return read_message<bool>(message, "new_randomizer_route");
            }

            void send_new_randomizer_route(const std::weak_ptr<IMessageSystem>& messaging)
            {
                send_message(messaging, true, "new_randomizer_route");
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

        std::optional<RouteMessage> read_add_to_route(const Message& message)
        {
            return read_message<RouteMessage>(message, "add_to_route");
        }

        void send_add_to_route(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IItem>& item)
        {
            send_message(messaging, RouteMessage{ .element = item }, "add_to_route");
        }

        void send_add_to_route(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ITrigger>& trigger)
        {
            send_message(messaging, RouteMessage{ .element = trigger }, "add_to_route");
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

        std::optional<std::string> read_open_level_filename(const Message& message)
        {
            return read_message<std::string>(message, "open_level_filename");
        }

        void send_open_level_filename(const std::weak_ptr<IMessageSystem>& messaging, const std::string& path)
        {
            send_message(messaging, path, "open_level_filename");
        }

        std::optional<std::string> read_switch_level_filename(const Message& message)
        {
            return read_message<std::string>(message, "switch_level_filename");
        }

        void send_switch_level_filename(const std::weak_ptr<IMessageSystem>& messaging, const std::string& path)
        {
            send_message(messaging, path, "switch_level_filename");
        }

        std::optional<std::weak_ptr<ILevel>> read_end_diff(const Message& message)
        {
            return read_message<std::weak_ptr<ILevel>>(message, "end_diff");
        }

        void send_end_diff(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ILevel>& level)
        {
            send_message(messaging, level, "end_diff");
        }

        std::optional<std::weak_ptr<ISector>> read_hover_sector(const Message& message)
        {
            return read_message<std::weak_ptr<ISector>>(message, "hover_sector");
        }

        void send_hover_sector(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ISector>& sector)
        {
            send_message(messaging, sector, "hover_sector");
        }

        void get_route(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to)
        {
            get_message(messaging, reply_to, "get_route");
        }

        std::optional<std::weak_ptr<IRoute>> read_route(const Message& message)
        {
            return read_message<std::weak_ptr<IRoute>>(message, "route");
        }

        void send_route(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRoute>& route)
        {
            send_message(messaging, route, "route");
        }

        std::optional<bool> read_route_window_opened(const Message& message)
        {
            return read_message<bool>(message, "route_window_opened");
        }

        void send_route_window_opened(const std::weak_ptr<IMessageSystem>& messaging)
        {
            send_message(messaging, true, "route_window_opened");
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

        void get_selected_waypoint(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to)
        {
            get_message(messaging, reply_to, "get_selected_waypoint");
        }

        std::optional<std::weak_ptr<IWaypoint>> read_select_waypoint(const Message& message)
        {
            return read_message<std::weak_ptr<IWaypoint>>(message, "select_waypoint");
        }

        void send_select_waypoint(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IWaypoint>& waypoint)
        {
            send_message(messaging, waypoint, "select_waypoint");
        }
    }
}
