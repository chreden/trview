#pragma once

#include <memory>
#include <trview.common/Event.h>

namespace trview
{
    struct ICameraSink;
    struct IDiffWindow;
    struct IItem;
    struct ILevel;
    struct ILight;
    struct ISoundSource;
    struct IStaticMesh;
    struct ITrigger;
    struct IRoom;
    struct ISector;
    struct UserSettings;

    struct IDiffWindowManager
    {
        virtual ~IDiffWindowManager() = 0;
        virtual std::weak_ptr<IDiffWindow> create_window() = 0;
        virtual void render() = 0;
        virtual void set_level(const std::weak_ptr<ILevel>& level) = 0;
        virtual void set_settings(const UserSettings& settings) = 0;

        Event<std::weak_ptr<IItem>> on_item_selected;
        Event<std::weak_ptr<ILight>> on_light_selected;
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;
        Event<std::weak_ptr<ILevel>> on_diff_ended;
        Event<std::weak_ptr<ICameraSink>> on_camera_sink_selected;
        Event<std::weak_ptr<IStaticMesh>> on_static_mesh_selected;
        Event<std::weak_ptr<ISoundSource>> on_sound_source_selected;
        Event<std::weak_ptr<IRoom>> on_room_selected;
        Event<std::weak_ptr<ISector>> on_sector_selected;
        Event<UserSettings> on_settings;
    };
}
