#pragma once

#include <memory>
#include <trview.common/Event.h>

namespace trview
{
    struct ICameraSink;
    struct IItem;
    struct ILevel;
    struct ILight;
    struct IRoom;
    struct IRoute;
    struct ISector;
    struct ISoundSource;
    struct IStaticMesh;
    struct ITrigger;
    struct IWaypoint;
    struct UserSettings;

    struct IWindows
    {
        virtual ~IWindows() = 0;
        virtual bool is_route_window_open() const = 0;
        virtual void update(float elapsed) = 0;
        virtual void render() = 0;
        virtual void select(const std::weak_ptr<ICameraSink>& camera_sink) = 0;
        virtual void select(const std::weak_ptr<IItem>& item) = 0;
        virtual void select(const std::weak_ptr<ILight>& light) = 0;
        virtual void select(const std::weak_ptr<ISoundSource>& sound_source) = 0;
        virtual void select(const std::weak_ptr<IStaticMesh>& static_mesh) = 0;
        virtual void select(const std::weak_ptr<ITrigger>& trigger) = 0;
        virtual void select(const std::weak_ptr<IWaypoint>& waypoint) = 0;
        virtual void set_level(const std::weak_ptr<ILevel>& level) = 0;
        virtual void set_room(const std::weak_ptr<IRoom>& room) = 0;
        virtual void set_route(const std::weak_ptr<IRoute>& route) = 0;
        virtual void set_settings(const UserSettings& settings) = 0;
        virtual void setup(const UserSettings& settings) = 0;

        Event<std::weak_ptr<ICameraSink>> on_camera_sink_selected;
        Event<std::weak_ptr<ILevel>> on_diff_ended;
        Event<std::weak_ptr<IItem>> on_item_selected;
        Event<std::string> on_level_switch;
        Event<std::weak_ptr<ILight>> on_light_selected;
        Event<> on_new_route;
        Event<> on_new_randomizer_route;
        Event<std::weak_ptr<IRoom>> on_room_selected;
        Event<> on_route_open;
        Event<> on_route_reload;
        Event<> on_route_save;
        Event<> on_route_save_as;
        Event<std::weak_ptr<ISector>> on_sector_hover;
        Event<std::weak_ptr<ISoundSource>> on_sound_source_selected;
        Event<std::weak_ptr<IStaticMesh>> on_static_selected;
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;
        Event<std::weak_ptr<IWaypoint>> on_waypoint_selected;
        Event<std::weak_ptr<ISector>> on_sector_selected;
        Event<> on_route_window_created;
        Event<> on_scene_changed;
        Event<UserSettings> on_settings;
    };
}
