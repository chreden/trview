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
                    lua_pushstring(L, trlevel::to_string(light->type()).c_str());
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
                    light->set_visible(lua_toboolean(L, -1));
                    return 0;
                }

                return 0;
            }
        }

        int create_light(lua_State* L, const std::shared_ptr<ILight>& light)
        {
            return create(L, light, light_index, light_newindex);
        }
    }
}
