#include "Lua.h"
#include <trview.common/Strings.h>
#include "trview/trview.h"
#include <external/lua/src/lualib.h>
#include <trview.lua.imgui/inc/trview.lua.imgui.h>
#include "Route/Lua_Waypoint.h"

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

        int dofile(lua_State* L)
        {
            luaL_checktype(L, lua_upvalueindex(1), LUA_TUSERDATA);
            ILua* self = *static_cast<ILua**>(lua_touserdata(L, lua_upvalueindex(1)));
            self->do_file(lua_tostring(L, 1));
            return 0;
        }

        void nil_functions(lua_State* L, const std::string& lib, const std::vector<std::string>& names)
        {
            for (const auto& name : names)
            {
                lua_getglobal(L, lib.c_str());
                lua_pushnil(L);
                lua_setfield(L, -2, name.c_str());
            }
        }

        static const luaL_Reg loadedlibs[] = {
          {LUA_GNAME, luaopen_base},
          {LUA_LOADLIBNAME, luaopen_package},
          {LUA_COLIBNAME, luaopen_coroutine},
          {LUA_TABLIBNAME, luaopen_table},
          {LUA_IOLIBNAME, luaopen_io},
          {LUA_OSLIBNAME, luaopen_os},
          {LUA_STRLIBNAME, luaopen_string},
          {LUA_MATHLIBNAME, luaopen_math},
          {LUA_UTF8LIBNAME, luaopen_utf8},
          {NULL, NULL}
        };

        LUALIB_API void trview_luaL_openlibs(lua_State* L) {
            const luaL_Reg* lib;
            /* "require" functions from 'loadedlibs' and set results to global table */
            for (lib = loadedlibs; lib->func; lib++) {
                luaL_requiref(L, lib->name, lib->func, 1);
                lua_pop(L, 1);  /* remove lib */
            }
        }

    }

    ILua::~ILua()
    {
    }

    Lua::Lua(const IRoute::Source& route_source, const IRandomizerRoute::Source& randomizer_route_source, const IWaypoint::Source& waypoint_source, const IScriptable::Source& scriptable_source, const std::shared_ptr<IDialogs>& dialogs, const std::shared_ptr<IFiles>& files)
        : _route_source(route_source), _randomizer_route_source(randomizer_route_source), _waypoint_source(waypoint_source), _scriptable_source(scriptable_source), _dialogs(dialogs), _files(files)
    {
        create_state();
    }

    Lua::~Lua()
    {
        lua_close(L);
    }

    void Lua::do_file(const std::string& file)
    {
        const auto current_working_directory = _files->working_directory();
        _files->set_working_directory(_directory);

        if (luaL_dofile(L, file.c_str()) != LUA_OK)
        {
            if (lua_type(L, -1) == LUA_TSTRING)
            {
                on_print(lua_tostring(L, -1));
            }
            else
            {
                on_print("An error occurred");
            }
        }

        _files->set_working_directory(current_working_directory);
    }

    void Lua::execute(const std::string& command)
    {
        if (luaL_dostring(L, command.c_str()) != LUA_OK)
        {
            if (lua_type(L, -1) == LUA_TSTRING)
            {
                on_print(lua_tostring(L, -1));
            }
            else
            {
                on_print("An error occurred");
            }
        }
    }

    void Lua::initialise(IApplication* application)
    {
        create_state();
        ILua** userdata = static_cast<ILua**>(lua_newuserdata(L, sizeof(this)));
        *userdata = this;
        lua_pushcclosure(L, print, 1);
        lua_setglobal(L, "print");
        userdata = static_cast<ILua**>(lua_newuserdata(L, sizeof(this)));
        *userdata = this;
        lua_pushcclosure(L, dofile, 1);
        lua_setglobal(L, "dofile");
        lua::trview_register(L, application, _route_source, _randomizer_route_source, _waypoint_source, _scriptable_source, _dialogs, _files);
        lua::imgui_register(L);
    }

    void Lua::set_directory(const std::string& directory)
    {
        _directory = directory;
    }

    void Lua::create_state()
    {
        if (L)
        {
            lua_close(L);
            L = nullptr;
        }

        L = luaL_newstate();
        trview_luaL_openlibs(L);
        nil_functions(L, LUA_OSLIBNAME, { "execute", "exit", "remove", "rename", "setlocale" });
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