#pragma once

#include <vector>
#include <cstdint>
#include <functional>
#include <memory>

#include <trview.common/Event.h>
#include "../../Elements/Item.h"
#include "../../Elements/ISector.h"

namespace trview
{
    struct IRoom;

    struct IFloordataWindow
    {
        using Source = std::function<std::shared_ptr<IFloordataWindow>()>;

        Event<> on_window_closed;
        Event<std::weak_ptr<ISector>> on_sector_highlight;

        virtual ~IFloordataWindow() = 0;
        virtual void render() = 0;
        virtual void set_current_room(uint32_t room) = 0;
        virtual void set_floordata(const std::vector<uint16_t>& data) = 0;
        virtual void set_items(const std::vector<Item>& items) = 0;
        virtual void set_number(int32_t number) = 0;
        virtual void set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms) = 0;
        virtual void update(float delta) = 0;
    };
}
