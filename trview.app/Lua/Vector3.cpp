module;

#include <SimpleMath.h>
#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>

module trview.app:LuaVector3;

import :Lua;

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace lua
    {
        namespace
        {
            int vector3_metatable = LUA_NOREF;

            int vector3_new(lua_State* L)
            {
                int args = lua_gettop(L);
                switch (args)
                {
                case 0:
                    return create_vector3(L, Vector3());
                case 3:
                    return create_vector3(L,
                        Vector3(
                            static_cast<float>(lua_tonumber(L, 1)),
                            static_cast<float>(lua_tonumber(L, 2)),
                            static_cast<float>(lua_tonumber(L, 3))));
                }
                return luaL_error(L, "Vector3.new expects 0 or 3 number arguments");
            }

            int vector3_constructor(lua_State* L)
            {
                int args = lua_gettop(L);
                switch (args)
                {
                case 1:
                    return create_vector3(L, Vector3());
                case 4:
                    return create_vector3(L,
                        Vector3(
                            static_cast<float>(lua_tonumber(L, 2)),
                            static_cast<float>(lua_tonumber(L, 3)),
                            static_cast<float>(lua_tonumber(L, 4))));
                }
                return luaL_error(L, "Vector3 constructor expects 0 or 3 number arguments");
            }

            int vector3_class_index(lua_State* L)
            {
                const std::string key = lua_tostring(L, 2);
                if (key == "new")
                {
                    lua_pushcfunction(L, vector3_new);
                    return 1;
                }
                return 0;
            }

            const std::unordered_map<std::string, lua_CFunction> Functions
            {
                { "x", prop_getter<Vector3, &Vector3::x> },
                { "y", prop_getter<Vector3, &Vector3::y> },
                { "z", prop_getter<Vector3, &Vector3::z> },
                { "length", prop_getter<Vector3, &Vector3::Length> }
            };
        }

        int create_vector3(lua_State* L, const Vector3& value)
        {
            create_userdata(L, value);
            assign_metatable(L, vector3_metatable);
            return 1;
        }

        bool is_vector3(lua_State* L, int index)
        {
            return equal_metatable(L, index, vector3_metatable);
        }

        Vector3 to_vector3(lua_State* L, int index)
        {
            return get_userdata<Vector3>(L, index);
        }

        Vector3 to_vector3(lua_State* L, int index, const std::string& name, const Vector3& default_value)
        {
            if (LUA_TUSERDATA == lua_getfield(L, index, name.c_str()))
            {
                int field_index = lua_gettop(L);
                Vector3 value = *static_cast<Vector3*>(lua_touserdata(L, field_index));
                lua_pop(L, 1);
                return value;
            }
            else
            {
                lua_pop(L, 1);
            }

            return default_value;
        }

        void vector3_register(lua_State* L)
        {
            vector3_metatable = store_metatable(L,
                {
                    { "__index", default_index<Functions> },
                    { "__gc", default_gc<Vector3> }
                });

            lua_newtable(L);
            create_metatable(L,
                {
                    { "__call", vector3_constructor },
                    { "__index", vector3_class_index }
                });
            lua_setglobal(L, "Vector3");
        }
    }
}
