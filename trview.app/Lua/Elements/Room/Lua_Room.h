#pragma once

#include "../../../Elements/IRoom.h"

struct lua_State;

namespace trview
{
    namespace lua
    {
        void room_register(lua_State* L);
        int create_room(lua_State* L, std::shared_ptr<IRoom> room);
        std::shared_ptr<IRoom> to_room(lua_State* L, int index);
        std::shared_ptr<IRoom> to_room(lua_State* L, int index, const std::string& field_name);
    }
}
