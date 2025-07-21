#pragma once

#include "ICameraSinkWindow.h"
#include "../../Elements/ITrigger.h"

namespace trview
{
    struct ICameraSinkWindowManager
    {
        virtual ~ICameraSinkWindowManager() = 0;
        virtual std::weak_ptr<ICameraSinkWindow> create_window() = 0;
        virtual void render() = 0;
        virtual void set_camera_sinks(const std::vector<std::weak_ptr<ICameraSink>>& camera_sinks) = 0;
        virtual void set_flybys(const std::vector<std::weak_ptr<IFlyby>>& flybys) = 0;
        virtual void set_platform_and_version(const trlevel::PlatformAndVersion& platform_and_version) = 0;
        virtual void set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink) = 0;
        virtual void set_selected_flyby_node(const std::weak_ptr<IFlybyNode>& flyby_node) = 0;
        virtual void set_room(const std::weak_ptr<IRoom>& room) = 0;
        virtual void set_settings(const UserSettings& settings) = 0;
        virtual void update(float delta) = 0;

        Event<std::weak_ptr<ICameraSink>> on_camera_sink_selected;
        Event<std::weak_ptr<IFlybyNode>> on_flyby_node_selected;
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;
        Event<> on_scene_changed;
        Event<UserSettings> on_settings;
    };
}
