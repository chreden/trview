#pragma once

#include <memory>
#include <trview.common/Event.h>

namespace trview
{
    struct IDiffWindow;
    struct ILevel;
    struct IItem;

    struct IDiffWindowManager
    {
        virtual ~IDiffWindowManager() = 0;
        virtual std::weak_ptr<IDiffWindow> create_window() = 0;
        virtual void render() = 0;
        virtual void set_level(const std::weak_ptr<ILevel>& level) = 0;

        Event<std::weak_ptr<IItem>> on_item_selected;
    };
}
