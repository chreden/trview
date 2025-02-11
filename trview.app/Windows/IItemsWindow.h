#pragma once

#include <vector>
#include <optional>
#include <trview.common/Event.h>
#include "../Elements/IItem.h"
#include "../Elements/ITrigger.h"
#include "../Filters/Filters.h"

namespace trview
{
    struct IItemsWindow
    {
        using Source = std::function<std::shared_ptr<IItemsWindow>()>;

        Event<std::weak_ptr<IItem>> on_add_to_route;
        Event<std::weak_ptr<IItem>> on_item_selected;
        Event<> on_scene_changed;
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;
        Event<> on_window_closed;

        virtual ~IItemsWindow() = 0;
        virtual void add_level(const std::weak_ptr<ILevel>& level) = 0;
        virtual std::string name() const = 0;
        virtual void render() = 0;
        virtual void set_current_room(const std::weak_ptr<IRoom>& room) = 0;
        virtual void set_filters(const std::weak_ptr<ILevel>& level, std::vector<Filters<IItem>::Filter> filters) = 0;
        virtual void set_number(int32_t number) = 0;
        virtual void set_selected_item(const std::weak_ptr<IItem>& item) = 0;
        virtual void update(float delta) = 0;
    };
}

