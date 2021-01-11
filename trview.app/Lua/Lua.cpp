#include "Lua.h"
#include <trview.common/Strings.h>

namespace trview
{
    namespace
    {
        int l_print(lua_State* L) 
        {
            lua_getglobal(L, "_trview_lua");
            Lua* lua = const_cast<Lua*>(static_cast<const Lua*>(lua_topointer(L, -1)));
            lua_pop(L, 1);

            std::string text;

            int nargs = lua_gettop(L);
            for (int i = 1; i <= nargs; i++) 
            {
                if (lua_isboolean(L, i))
                {
                    text += lua_toboolean(L, i) ? "true" : "false";
                }
                else
                {
                    text += lua_tostring(L, i);
                }

                if (i < nargs)
                {
                    text += " ";
                }
            }
            lua->print(text);
            return 0;
        }
    }

    Lua::Lua()
    {
        _lua = luaL_newstate();
        luaL_openlibs(_lua);

        // Register class instance so print can get it.
        lua_pushlightuserdata(_lua, this);
        lua_setglobal(_lua, "_trview_lua");

        // Replace print so it goes to the trview console.
        luaL_Reg print_library[] = {{"print", l_print}, {NULL, NULL}};
        lua_getglobal(_lua, "_G");
        luaL_setfuncs(_lua, print_library, 0);
        lua_pop(_lua, 1);
    }

    void Lua::execute(const std::string& command)
    {
        luaL_dostring(_lua, command.c_str());
    }

    void Lua::print(const std::string& value)
    {
        on_text(to_utf16(value));
    }

    lua_State* Lua::state() const
    {
        return _lua;
    }
}
