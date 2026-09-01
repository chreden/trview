export module trview.app:Messages;

import std;

import trview.common;

namespace trview
{
    struct IItem;
    struct ICameraSink;
    struct IFlybyNode;
    struct ILevel;
    struct ILight;
    struct IRecipient;
    struct IRoom;
    struct IRoute;
    struct ISector;
    struct ISoundSource;
    struct IStaticMesh;
    struct ITrigger;
    struct IWaypoint;
    struct UserSettings;

    namespace messages
    {
        export struct RouteMessage
        {
            // TODO: Interfaces
            std::variant<std::weak_ptr<ITrigger>, std::weak_ptr<IItem>, std::weak_ptr<ILight>, std::weak_ptr<ICameraSink>> element;
        };

        namespace commands
        {
            export std::optional<bool> read_route_open(const Message& message);
            export void send_route_open(const std::weak_ptr<IMessageSystem>& messaging);

            export std::optional<bool> read_route_reload(const Message& message);
            export void send_route_reload(const std::weak_ptr<IMessageSystem>& messaging);

            export std::optional<bool> read_route_save(const Message& message);
            export void send_route_save(const std::weak_ptr<IMessageSystem>& messaging);

            export std::optional<bool> read_route_save_as(const Message& message);
            export void send_route_save_as(const std::weak_ptr<IMessageSystem>& messaging);

            export std::optional<bool> read_new_route(const Message& message);
            export void send_new_route(const std::weak_ptr<IMessageSystem>& messaging);

            export std::optional<bool> read_new_randomizer_route(const Message& message);
            export void send_new_randomizer_route(const std::weak_ptr<IMessageSystem>& messaging);

            export std::optional<bool> read_unhide_all(const Message& message);
            export void send_unhide_all(const std::weak_ptr<IMessageSystem>& messaging);
        }

        export void get_settings(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        export std::optional<UserSettings> read_settings(const Message& message);
        export void send_settings(const std::weak_ptr<IMessageSystem>& messaging, const UserSettings& settings);

        export std::optional<RouteMessage> read_add_to_route(const Message& message);
        export void send_add_to_route(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IItem>& item);
        export void send_add_to_route(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ITrigger>& trigger);
        export void send_add_to_route(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ILight>& light);
        export void send_add_to_route(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ICameraSink>& camera_sink);

        export std::optional<bool> read_ng_plus(const Message& message);
        export void send_ng_plus(const std::weak_ptr<IMessageSystem>& messaging, bool value);

        export void get_open_level(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        export std::optional<std::weak_ptr<ILevel>> read_open_level(const Message& message);
        export void send_open_level(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ILevel>& level);

        export std::optional<std::string> read_open_level_filename(const Message& message);
        export void send_open_level_filename(const std::weak_ptr<IMessageSystem>& messaging, const std::string& path);

        export std::optional<std::string> read_switch_level_filename(const Message& message);
        export void send_switch_level_filename(const std::weak_ptr<IMessageSystem>& messaging, const std::string& path);

        export std::optional<std::weak_ptr<ILevel>> read_end_diff(const Message& message);
        export void send_end_diff(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ILevel>& level);

        export std::optional<std::weak_ptr<ISector>> read_hover_sector(const Message& message);
        export void send_hover_sector(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ISector>& sector);

        export void get_route(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        export std::optional<std::weak_ptr<IRoute>> read_route(const Message& message);
        export void send_route(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRoute>& sector);

        export std::optional<bool> read_route_window_opened(const Message& message);
        export void send_route_window_opened(const std::weak_ptr<IMessageSystem>& messaging);

        export void get_selected_camera_sink(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        export std::optional<std::weak_ptr<ICameraSink>> read_select_camera_sink(const Message& message);
        export void send_select_camera_sink(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ICameraSink>& camera_sink);

        export void get_selected_item(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        export std::optional<std::weak_ptr<IItem>> read_select_item(const Message& message);
        export void send_select_item(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IItem>& item);

        export void get_selected_flyby_node(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        export std::optional<std::weak_ptr<IFlybyNode>> read_select_flyby_node(const Message& message);
        export void send_select_flyby_node(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IFlybyNode>& flyby_node);

        export void get_selected_light(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        export std::optional<std::weak_ptr<ILight>> read_select_light(const Message& message);
        export void send_select_light(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ILight>& light);

        export void get_selected_room(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        export std::optional<std::weak_ptr<IRoom>> read_select_room(const Message& message);
        export void send_select_room(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRoom>& room);

        export void get_selected_sector(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        export std::optional<std::weak_ptr<ISector>> read_select_sector(const Message& message);
        export void send_select_sector(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ISector>& sector);

        export void get_selected_sound_source(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        export std::optional<std::weak_ptr<ISoundSource>> read_select_sound_source(const Message& message);
        export void send_select_sound_source(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ISoundSource>& sound_source);

        export void get_selected_static_mesh(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        export std::optional<std::weak_ptr<IStaticMesh>> read_select_static_mesh(const Message& message);
        export void send_select_static_mesh(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IStaticMesh>& static_mesh);

        export void get_selected_trigger(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        export std::optional<std::weak_ptr<ITrigger>> read_select_trigger(const Message& message);
        export void send_select_trigger(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ITrigger>& trigger);

        export void get_selected_waypoint(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        export std::optional<std::weak_ptr<IWaypoint>> read_select_waypoint(const Message& message);
        export void send_select_waypoint(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IWaypoint>& trigger);

        export template <typename T>
        void reply_to(const Message& message, const std::string& type, T&& data);
    }
}

#include "Messages.inl"
