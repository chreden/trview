#pragma once

#include <optional>
#include <memory>
#include <variant>

namespace trview
{
    struct IItem;
    struct ICameraSink;
    struct IFlybyNode;
    struct ILevel;
    struct ILight;
    struct IMessageSystem;
    struct IRecipient;
    struct IRoom;
    struct IRoute;
    struct ISector;
    struct ISoundSource;
    struct IStaticMesh;
    struct ITrigger;
    struct IWaypoint;
    struct Message;
    struct UserSettings;

    namespace messages
    {
        struct RouteMessage
        {
            // TODO: Interfaces
            std::variant<std::weak_ptr<ITrigger>, std::weak_ptr<IItem>> element;
        };

        namespace commands
        {
            std::optional<bool> read_route_open(const Message& message);
            void send_route_open(const std::weak_ptr<IMessageSystem>& messaging);

            std::optional<bool> read_route_reload(const Message& message);
            void send_route_reload(const std::weak_ptr<IMessageSystem>& messaging);

            std::optional<bool> read_route_save(const Message& message);
            void send_route_save(const std::weak_ptr<IMessageSystem>& messaging);

            std::optional<bool> read_route_save_as(const Message& message);
            void send_route_save_as(const std::weak_ptr<IMessageSystem>& messaging);

            std::optional<bool> read_new_route(const Message& message);
            void send_new_route(const std::weak_ptr<IMessageSystem>& messaging);

            std::optional<bool> read_new_randomizer_route(const Message& message);
            void send_new_randomizer_route(const std::weak_ptr<IMessageSystem>& messaging);

            std::optional<bool> read_unhide_all(const Message& message);
            void send_unhide_all(const std::weak_ptr<IMessageSystem>& messaging);
        }

        void get_settings(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        std::optional<UserSettings> read_settings(const Message& message);
        void send_settings(const std::weak_ptr<IMessageSystem>& messaging, const UserSettings& settings);

        std::optional<RouteMessage> read_add_to_route(const Message& message);
        void send_add_to_route(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IItem>& item);
        void send_add_to_route(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ITrigger>& trigger);

        std::optional<bool> read_ng_plus(const Message& message);
        void send_ng_plus(const std::weak_ptr<IMessageSystem>& messaging, bool value);

        void get_open_level(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        std::optional<std::weak_ptr<ILevel>> read_open_level(const Message& message);
        void send_open_level(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ILevel>& level);

        std::optional<std::string> read_open_level_filename(const Message& message);
        void send_open_level_filename(const std::weak_ptr<IMessageSystem>& messaging, const std::string& path);

        std::optional<std::string> read_switch_level_filename(const Message& message);
        void send_switch_level_filename(const std::weak_ptr<IMessageSystem>& messaging, const std::string& path);

        std::optional<std::weak_ptr<ILevel>> read_end_diff(const Message& message);
        void send_end_diff(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ILevel>& level);

        std::optional<std::weak_ptr<ISector>> read_hover_sector(const Message& message);
        void send_hover_sector(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ISector>& sector);

        void get_route(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        std::optional<std::weak_ptr<IRoute>> read_route(const Message& message);
        void send_route(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRoute>& sector);

        std::optional<bool> read_route_window_opened(const Message& message);
        void send_route_window_opened(const std::weak_ptr<IMessageSystem>& messaging);

        void get_selected_camera_sink(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        std::optional<std::weak_ptr<ICameraSink>> read_select_camera_sink(const Message& message);
        void send_select_camera_sink(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ICameraSink>& camera_sink);

        void get_selected_item(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        std::optional<std::weak_ptr<IItem>> read_select_item(const Message& message);
        void send_select_item(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IItem>& item);

        void get_selected_flyby_node(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        std::optional<std::weak_ptr<IFlybyNode>> read_select_flyby_node(const Message& message);
        void send_select_flyby_node(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IFlybyNode>& flyby_node);

        void get_selected_light(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        std::optional<std::weak_ptr<ILight>> read_select_light(const Message& message);
        void send_select_light(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ILight>& light);

        void get_selected_room(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        std::optional<std::weak_ptr<IRoom>> read_select_room(const Message& message);
        void send_select_room(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRoom>& room);

        void get_selected_sector(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        std::optional<std::weak_ptr<ISector>> read_select_sector(const Message& message);
        void send_select_sector(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ISector>& sector);

        void get_selected_sound_source(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        std::optional<std::weak_ptr<ISoundSource>> read_select_sound_source(const Message& message);
        void send_select_sound_source(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ISoundSource>& sound_source);

        void get_selected_static_mesh(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        std::optional<std::weak_ptr<IStaticMesh>> read_select_static_mesh(const Message& message);
        void send_select_static_mesh(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IStaticMesh>& static_mesh);

        void get_selected_trigger(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        std::optional<std::weak_ptr<ITrigger>> read_select_trigger(const Message& message);
        void send_select_trigger(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<ITrigger>& trigger);

        void get_selected_waypoint(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IRecipient>& reply_to);
        std::optional<std::weak_ptr<IWaypoint>> read_select_waypoint(const Message& message);
        void send_select_waypoint(const std::weak_ptr<IMessageSystem>& messaging, const std::weak_ptr<IWaypoint>& trigger);

        template <typename T>
        void reply_to(const Message& message, const std::string& type, T&& data);
    }
}

#include "Messages.inl"
