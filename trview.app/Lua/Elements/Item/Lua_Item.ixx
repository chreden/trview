module;

#include <external/lua/src/lua.h>

export module trview.app:LuaItem;

import std;
import :IItem;

namespace trview
{
    namespace lua
    {
        export int create_item(lua_State* L, const std::shared_ptr<IItem>& item);
        export std::shared_ptr<IItem> to_item(lua_State* L, int index);
    }
}