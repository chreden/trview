#pragma once

#include "IFloordataWindow.h"

namespace trview
{
    struct IFloordataWindowManager
    {
        Event<std::weak_ptr<ISector>> on_sector_highlight;

        virtual ~IFloordataWindowManager() = 0;
        virtual std::weak_ptr<IFloordataWindow> create_window() = 0;
        virtual void render() = 0;
        virtual void set_room(uint32_t room) = 0;
        virtual void set_floordata(const std::vector<uint16_t>& data) = 0;
        virtual void set_items(const std::vector<Item>& items) = 0;
        virtual void set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms) = 0;
        virtual void update(float delta) = 0;
    };
}
