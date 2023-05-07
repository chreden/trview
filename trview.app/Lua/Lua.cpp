#include "Lua.h"
#include <trview.common/Strings.h>
#include "trview/trview.h"
#include <external/lua/src/lualib.h>
#include <trview.lua.imgui/trview.lua.imgui.h>

namespace trview
{
    namespace
    {
        int print(lua_State* L)
        {
            luaL_checktype(L, lua_upvalueindex(1), LUA_TUSERDATA);
            ILua* self = *static_cast<ILua**>(lua_touserdata(L, lua_upvalueindex(1)));

            std::ostringstream oss;

            int nargs = lua_gettop(L);
            for (int i = 1; i <= nargs; ++i)
            {
                if (lua_isboolean(L, i))
                {
                    oss << (lua_toboolean(L, i) ? "true" : "false");
                }
                else if (lua_isnoneornil(L, i))
                {
                    oss << "nil";
                }
                else
                {
                    const char* const s = luaL_checkstring(L, i);
                    if (s)
                        oss << lua_tostring(L, i);
                }

                if (i != nargs)
                {
                    oss << " ";
                }
            }

            self->on_print(oss.str());
            return 0;
        }
    }

    ILua::~ILua()
    {
    }

    Lua::Lua(const IRoute::Source& route_source, const IWaypoint::Source& waypoint_source)
        : _route_source(route_source), _waypoint_source(waypoint_source)
    {
        L = luaL_newstate();
        luaL_openlibs(L);
    }

    Lua::~Lua()
    {
        lua_close(L);
    }

    void Lua::do_file(const std::string& file)
    {
        if (luaL_dofile(L, file.c_str()) != LUA_OK)
        {
            std::string msg = lua_tostring(L, -1);
            on_print(msg);
        }
    }

    void Lua::execute(const std::string& command)
    {
        if (luaL_dostring(L, command.c_str()) != LUA_OK)
        {
            std::string msg = lua_tostring(L, -1);
            on_print(msg);
        }
    }

    void Lua::initialise(IApplication* application)
    {
        ILua** userdata = static_cast<ILua**>(lua_newuserdata(L, sizeof(this)));
        *userdata = this;
        lua_pushcclosure(L, print, 1);
        lua_setglobal(L, "print");
        lua::trview_register(L, application, _route_source, _waypoint_source);
        lua::imgui_register(L);
    }

    namespace lua
    {
        int push_string(lua_State* L, const std::string& text)
        {
            lua_pushstring(L, text.c_str());
            return 1;
        }
    }
}