module;

#include <external/lua/src/lua.h>

export module trview.app:LuaRoom;

import std;
import :IRoom;

namespace trview
{
    namespace lua
    {
        export void room_register(lua_State* L);
        export int create_room(lua_State* L, std::shared_ptr<IRoom> room);
        export std::shared_ptr<IRoom> to_room(lua_State* L, int index);
        export std::shared_ptr<IRoom> to_room(lua_State* L, int index, const std::string& field_name);
    }
}
