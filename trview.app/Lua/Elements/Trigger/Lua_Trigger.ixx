module;

#include <external/lua/src/lua.h>

export module trview.app:LuaTrigger;

import std;
import :ITrigger;

namespace trview
{
    namespace lua
    {
        export void trigger_register(lua_State* L);
        export int create_trigger(lua_State* L, const std::shared_ptr<ITrigger>& trigger);
        export std::shared_ptr<ITrigger> to_trigger(lua_State* L, int index);
    }
}
