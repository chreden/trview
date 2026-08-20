#include "Colour.h"
#include "Lua.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace lua
    {
        namespace
        {
            int colour_metatable = LUA_NOREF;

            int colour_new(lua_State* L)
            {
                int args = lua_gettop(L);
                switch (args)
                {
                case 0:
                    return create_colour(L, Colour());
                case 3:
                    return create_colour(L,
                        Colour(
                            static_cast<float>(lua_tonumber(L, 1)),
                            static_cast<float>(lua_tonumber(L, 2)),
                            static_cast<float>(lua_tonumber(L, 3))));
                case 4:
                    return create_colour(L,
                        Colour(
                            static_cast<float>(lua_tonumber(L, 4)),
                            static_cast<float>(lua_tonumber(L, 1)),
                            static_cast<float>(lua_tonumber(L, 2)),
                            static_cast<float>(lua_tonumber(L, 3))));
                }
                return luaL_error(L, "Colour expects 0, 3 or 4 number arguments");
            }

            int colour_constructor(lua_State* L)
            {
                int args = lua_gettop(L);
                switch (args)
                {
                case 1:
                    return create_colour(L, Colour());
                case 4:
                    return create_colour(L,
                        Colour(
                            static_cast<float>(lua_tonumber(L, 2)),
                            static_cast<float>(lua_tonumber(L, 3)),
                            static_cast<float>(lua_tonumber(L, 4))));
                case 5:
                    return create_colour(L,
                        Colour(
                            static_cast<float>(lua_tonumber(L, 5)),
                            static_cast<float>(lua_tonumber(L, 2)),
                            static_cast<float>(lua_tonumber(L, 3)),
                            static_cast<float>(lua_tonumber(L, 4))));
                }
                return luaL_error(L, "Colour constructor expects 0, 3 or 4 number arguments");
            }

            int colour_class_index(lua_State* L)
            {
                const std::string key = lua_tostring(L, 2);
                if (key == "new")
                {
                    lua_pushcfunction(L, colour_new);
                    return 1;
                }
                return 0;
            }

            int colour_index(lua_State* L)
            {
                const auto& self = get_userdata<Colour>(L, 1);
                const std::string key = lua_tostring(L, 2);
                if (key == "r")
                {
                    lua_pushnumber(L, self.r);
                    return 1;
                }
                else if (key == "g")
                {
                    lua_pushnumber(L, self.g);
                    return 1;
                }
                else if (key == "b")
                {
                    lua_pushnumber(L, self.b);
                    return 1;
                }
                else if (key == "a")
                {
                    lua_pushnumber(L, self.a);
                    return 1;
                }
                return 0;
            }

            int colour_gc(lua_State* L)
            {
                cleanup_userdata<Colour>(L, 1);
                return 0;
            }
        }

        int create_colour(lua_State* L, const Colour& value)
        {
            create_userdata(L, value);
            assign_metatable(L, colour_metatable);
            return 1;
        }

        bool is_colour(lua_State* L, int index)
        {
            return equal_metatable(L, index, colour_metatable);
        }

        Colour to_colour(lua_State* L, int index)
        {
            const int field_index = index < 0 ? (index + lua_gettop(L) + 1) : index;
            return *static_cast<Colour*>(lua_touserdata(L, field_index));
        }

        int colour_new(lua_State* L)
        {
            float r = static_cast<float>(lua_tonumber(L, 1));
            float g = static_cast<float>(lua_tonumber(L, 2));
            float b = static_cast<float>(lua_tonumber(L, 3));
            float a = lua_gettop(L) >= 4 ? static_cast<float>(lua_tonumber(L, 4)) : 1.0f;
            return create_colour(L, Colour(a, r, g, b));
        }

        void colour_register(lua_State* L)
        {
            colour_metatable = store_metatable(L,
                {
                    { "__index", colour_index },
                    { "__gc", colour_gc }
                });

            lua_newtable(L);
            create_metatable(L,
                {
                    { "__call", colour_constructor },
                    { "__index", colour_class_index }
                });
            lua_setglobal(L, "Colour");
        }
    }
}
