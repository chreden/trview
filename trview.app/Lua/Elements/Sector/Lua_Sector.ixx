module;

#include <external/lua/src/lua.h>

export module trview.app:LuaSector;

import std;
import :ISector;

namespace trview
{
    namespace lua
    {
        export void sector_register(lua_State* L);
        export std::shared_ptr<ISector> to_sector(lua_State* L, int index);
        export int to_lua(lua_State* L, const std::weak_ptr<ISector>& sector);
    }

    export using lua::to_lua;
}
