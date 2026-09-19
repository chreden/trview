module;

#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include <SimpleMath.h>

module trview.app:LuaLight;

import trview.lua;

import trlevel;
import :Lua;
import :LuaRoom;
import :LuaVector3;
import :LuaColour;

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace lua
    {
        namespace
        {
            int light_metatable = LUA_NOREF;

            int light_index(lua_State* L)
            {
                auto light = lua::get_userdata<std::shared_ptr<ILight>>(L, 1);

                const std::string key = lua_tostring(L, 2);
                if (key == "cutoff")
                {
                    lua_pushnumber(L, cutoff(*light));
                    return 1;
                }
                else if (key == "density")
                {
                    lua_pushnumber(L, density(*light));
                    return 1;
                }
                else if (key == "fade")
                {
                    lua_pushnumber(L, fade(*light));
                    return 1;
                }
                else if (key == "falloff")
                {
                    lua_pushnumber(L, falloff(*light));
                    return 1;
                }
                else if (key == "falloff_angle")
                {
                    lua_pushnumber(L, falloff_angle(*light));
                    return 1;
                }
                else if (key == "hotspot")
                {
                    lua_pushnumber(L, hotspot(*light));
                    return 1;
                }
                else if (key == "intensity")
                {
                    lua_pushnumber(L, intensity(*light));
                    return 1;
                }
                else if (key == "length")
                {
                    lua_pushnumber(L, length(*light));
                    return 1;
                }
                else if (key == "radius")
                {
                    lua_pushnumber(L, radius(*light));
                    return 1;
                }
                else if (key == "rad_in")
                {
                    lua_pushnumber(L, rad_in(*light));
                    return 1;
                }
                else if (key == "rad_out")
                {
                    lua_pushnumber(L, rad_out(*light));
                    return 1;
                }
                else if (key == "range")
                {
                    lua_pushnumber(L, range(*light));
                    return 1;
                }
                else if (key == "type")
                {
                    lua_pushstring(L, trlevel::to_string(light->type()).c_str());
                    return 1;
                }

                return 0;
            }

            int light_newindex(lua_State* L)
            {
                auto light = lua::get_userdata<std::shared_ptr<ILight>>(L, 1);

                const std::string key = lua_tostring(L, 2);
                if (key == "visible")
                {
                    light->set_visible(lua_toboolean(L, -1));
                    return 0;
                }

                return 0;
            }

            DirectX::SimpleMath::Vector3 scale_vector(const DirectX::SimpleMath::Vector3& v)
            {
                return v * trlevel::Scale;
            }

            const std::unordered_map<std::string, lua_CFunction> Functions
            {
                { "colour", prop_getter<std::shared_ptr<ILight>, &ILight::colour> },
                { "direction", prop_getter<std::shared_ptr<ILight>, &ILight::direction> },
                { "number", prop_getter<std::shared_ptr<ILight>, &ILight::number> },
                { "position", prop_getter_with_transform<std::shared_ptr<ILight>, &ILight::position, scale_vector> },
                { "room", prop_getter<std::shared_ptr<ILight>, &ILight::room> },
                { "visible", prop_getter<std::shared_ptr<ILight>, &ILight::visible> },
            };
        }

        void light_register(lua_State* L)
        {
            light_metatable = store_metatable(L,
                {
                    { "__index", default_index<Functions, light_index> },
                    { "__newindex", light_newindex },
                    { "__gc", default_gc<std::shared_ptr<ILight>> }
                });
        }

        int create_light(lua_State* L, const std::shared_ptr<ILight>& light)
        {
            return create_userdata(L, light, light_metatable);
        }

        int to_lua(lua_State* L, const std::weak_ptr<ILight>& light)
        {
            return create_light(L, light.lock());
        }
    }
}
