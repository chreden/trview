#pragma once

#include <trview.common/Event.h>
#include "ItemsWindow.h"

namespace trview
{
    struct IItemsWindowManager
    {
        Event<std::weak_ptr<IItem>> on_item_selected;
        Event<> on_scene_changed;
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;
        Event<std::weak_ptr<IItem>> on_add_to_route;

        virtual ~IItemsWindowManager() = 0;
        virtual void add_level(const std::weak_ptr<ILevel>& level) = 0;
        virtual std::weak_ptr<IItemsWindow> create_window() = 0;
        virtual void render() = 0;
        virtual void set_room(const std::weak_ptr<IRoom>& room) = 0;
        virtual void set_selected_item(const std::weak_ptr<IItem>& item) = 0;
        virtual void update(float delta) = 0;
        virtual std::vector<std::weak_ptr<IItemsWindow>> windows() const = 0;
    };
}
