#pragma once

#include "../Messages/Messages.h"

namespace trview
{
    template <typename T>
    void Level::sync_room(const std::shared_ptr<T>& element) const
    {
        std::weak_ptr<IRoom> room;
        if (element)
        {
            room = element->room();
        }
        messages::send_select_room(_messaging, room);
    }
}
