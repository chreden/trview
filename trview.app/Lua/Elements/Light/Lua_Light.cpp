#include "Lua_Light.h"
#include "../../Lua.h"
#include "../../../Elements/ILevel.h"
#include "../Level/Lua_Level.h"
#include "../Room/Lua_Room.h"
#include "../../Vector3.h"
#include "../../Colour.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            std::unordered_map<ILight**, std::shared_ptr<ILight>> lights;

            int light_index(lua_State* L)
            {
                auto light = lua::get_self<ILight>(L);

                const std::string key = lua_tostring(L, 2);
                if (key == "colour")
                {
                    return create_colour(L, light->colour());
                }
                else if (key == "cutoff")
                {
                    lua_pushnumber(L, cutoff(*light));
                    return 1;
                }
                else if (key == "density")
                {
                    lua_pushnumber(L, density(*light));
                    return 1;
                }
                else if (key == "direction")
                {
                    return create_vector3(L, light->direction());
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
                else if (key == "number")
                {
                    lua_pushinteger(L, light->number());
                    return 1;
                }
                else if (key == "position")
                {
                    return create_vector3(L, light->position() * trlevel::Scale);
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
                else if (key == "room")
                {
                    return create_room(L, light->room().lock());
                }
                else if (key == "type")
                {
                    lua_pushstring(L, trlevel::light_type_name(light->type()).c_str());
                    return 1;
                }
                else if (key == "visible")
                {
                    lua_pushboolean(L, light->visible());
                    return 1;
                }

                return 0;
            }

            int light_newindex(lua_State* L)
            {
                auto light = lua::get_self<ILight>(L);

                const std::string key = lua_tostring(L, 2);
                if (key == "visible")
                {
                    if (auto level = light->level().lock())
                    {
                        level->set_light_visibility(light->number(), lua_toboolean(L, -1));
                    }
                    return 0;
                }

                return 0;
            }

            int light_gc(lua_State* L)
            {
                luaL_checktype(L, 1, LUA_TUSERDATA);
                ILight** userdata = static_cast<ILight**>(lua_touserdata(L, 1));
                lights.erase(userdata);
                return 0;
            }
        }

        void create_light(lua_State* L, const std::shared_ptr<ILight>& light)
        {
            if (!light)
            {
                lua_pushnil(L);
                return;
            }

            ILight** userdata = static_cast<ILight**>(lua_newuserdata(L, sizeof(light.get())));
            *userdata = light.get();
            lights[userdata] = light;

            lua_newtable(L);
            lua_pushcfunction(L, light_index);
            lua_setfield(L, -2, "__index");
            lua_pushcfunction(L, light_newindex);
            lua_setfield(L, -2, "__newindex");
            lua_pushcfunction(L, light_gc);
            lua_setfield(L, -2, "__gc");
            lua_setmetatable(L, -2);
        }
    }
}
