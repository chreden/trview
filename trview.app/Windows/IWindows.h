#pragma once

#include "../Elements/ILevel.h"
#include "../Settings/UserSettings.h"

namespace trview
{
    struct IWindows
    {
        virtual ~IWindows() = 0;
        virtual void render() = 0;
        virtual void set_item_visibility(const Item& item, bool value) = 0;
        virtual void set_level(std::shared_ptr<ILevel> level) = 0;
        virtual void set_room(uint32_t room) = 0;
        virtual void set_selected_camera_sink(std::weak_ptr<ICameraSink> camera_sink) = 0;
        virtual void set_selected_item(const Item& item) = 0;
        virtual void set_selected_light(std::weak_ptr<ILight> light) = 0;
        virtual void set_selected_trigger(std::weak_ptr<ITrigger> trigger) = 0;
        virtual void set_settings(const UserSettings& settings) = 0;
        virtual void startup(const UserSettings& settings) = 0;
        virtual void update(float elapsed) = 0;

        Event<std::weak_ptr<ITrigger>> on_add_to_route;
        Event<std::weak_ptr<ICameraSink>> on_camera_sink_selected;
        Event<Item> on_item_selected;
        Event<Item, bool> on_item_visibility;
        Event<std::weak_ptr<ILight>> on_light_selected;
        Event<std::weak_ptr<ILight>, bool> on_light_visibility;
        Event<uint32_t> on_room_selected;
        Event<std::weak_ptr<IRoom>, bool> on_room_visibility;
        Event<std::weak_ptr<ISector>> on_sector_selected;
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;
        Event<std::weak_ptr<ITrigger>, bool> on_trigger_visibility;
    };
}
