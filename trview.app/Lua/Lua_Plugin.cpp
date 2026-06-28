#include "Lua_Plugin.h"
#include "Lua.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace lua
    {
        void plugin_register(lua_State* L)
        {
            lua_newtable(L);
            // lua_pushcfunction(L, vector3_new);
            // lua_setfield(L, -2, "new");
            lua_setglobal(L, "Plugin");
        }
    }
}
