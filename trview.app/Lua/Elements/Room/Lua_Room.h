#pragma once

struct lua_State;

namespace trview
{
    struct ILevel;
    struct IRoom;

    namespace lua
    {
        void create_room(lua_State* L, const std::weak_ptr<IRoom>& room);
        void room_set_current_level(ILevel* level);
    }
}