#pragma once

#include "../../../Elements/IRoom.h"

struct lua_State;

namespace trview
{
    namespace lua
    {
        int create_room(lua_State* L, std::shared_ptr<IRoom> room);
    }
}
