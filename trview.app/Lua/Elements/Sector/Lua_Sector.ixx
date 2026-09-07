module;

#include <external/lua/src/lua.h>

export module trview.app:LuaSector;

import std;
import :ISector;

namespace trview
{
    namespace lua
    {
        export int create_sector(lua_State* L, std::shared_ptr<ISector> sector);
        export void sector_register(lua_State* L);
        export std::shared_ptr<ISector> to_sector(lua_State* L, int index);
    }
}
