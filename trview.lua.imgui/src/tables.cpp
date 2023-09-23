#include "pch.h"
#include "tables.h"

namespace trview
{
    namespace lua
    {
        float get_double(lua_State* L, int index, const std::string& name)
        {
            luaL_checktype(L, index, LUA_TTABLE);
            lua_getfield(L, index, name.c_str());
            luaL_checktype(L, -1, LUA_TNUMBER);
            double value = lua_tonumber(L, -1);
            lua_pop(L, 1);
            return value;
        }

        float get_float(lua_State* L, int index, const std::string& name)
        {
            luaL_checktype(L, index, LUA_TTABLE);
            lua_getfield(L, index, name.c_str());
            luaL_checktype(L, -1, LUA_TNUMBER);
            float value = lua_tonumber(L, -1);
            lua_pop(L, 1);
            return value;
        }

        int get_integer(lua_State* L, int index, const std::string& name)
        {
            luaL_checktype(L, index, LUA_TTABLE);
            lua_getfield(L, index, name.c_str());
            luaL_checktype(L, -1, LUA_TNUMBER);
            int value = lua_tointeger(L, -1);
            lua_pop(L, 1);
            return value;
        }

        std::optional<double> get_optional_double(lua_State* L, int index, const std::string& name)
        {
            luaL_checktype(L, index, LUA_TTABLE);
            lua_getfield(L, index, name.c_str());
            if (lua_isnil(L, -1))
            {
                lua_pop(L, 1);
                return std::nullopt;
            }
            luaL_checktype(L, -1, LUA_TNUMBER);
            double value = lua_tonumber(L, -1);
            lua_pop(L, 1);
            return value;
        }

        std::optional<float> get_optional_float(lua_State* L, int index, const std::string& name)
        {
            luaL_checktype(L, index, LUA_TTABLE);
            lua_getfield(L, index, name.c_str());
            if (lua_isnil(L, -1))
            {
                lua_pop(L, 1);
                return std::nullopt;
            }
            luaL_checktype(L, -1, LUA_TNUMBER);
            float value = lua_tonumber(L, -1);
            lua_pop(L, 1);
            return value;
        }

        std::optional<int> get_optional_integer(lua_State* L, int index, const std::string& name)
        {
            luaL_checktype(L, index, LUA_TTABLE);
            lua_getfield(L, index, name.c_str());
            if (lua_isnil(L, -1))
            {
                lua_pop(L, 1);
                return std::nullopt;
            }
            luaL_checktype(L, -1, LUA_TNUMBER);
            int value = lua_tointeger(L, -1);
            lua_pop(L, 1);
            return value;
        }

        std::optional<std::string> get_optional_string(lua_State* L, int index, const std::string& name)
        {
            luaL_checktype(L, index, LUA_TTABLE);
            lua_getfield(L, index, name.c_str());
            if (lua_isnil(L, -1))
            {
                lua_pop(L, 1);
                return std::nullopt;
            }
            luaL_checktype(L, -1, LUA_TSTRING);
            std::string value = lua_tostring(L, -1);
            lua_pop(L, 1);
            return value;
        }

        std::string get_string(lua_State* L, int index, const std::string& name)
        {
            luaL_checktype(L, index, LUA_TTABLE);
            lua_getfield(L, index, name.c_str());
            luaL_checktype(L, -1, LUA_TSTRING);
            std::string value = lua_tostring(L, -1);
            lua_pop(L, 1);
            return value;
        }

        void set_enum(lua_State* L, const std::string& name, int index, const std::vector<EnumValue>& values)
        {
            if (index < 0)
            {
                index += lua_gettop(L) + 1;
            }

            lua_newtable(L);
            for (const auto& v : values)
            {
                set_integer(L, -1, v.name.c_str(), v.value);
            }
            lua_setfield(L, index, name.c_str());
        }

        void set_integer(lua_State* L, int index, const std::string& name, int value)
        {
            if (index < 0)
            {
                index += lua_gettop(L) + 1;
            }
            lua_pushinteger(L, value);
            lua_setfield(L, index, name.c_str());
        }
    }
}
