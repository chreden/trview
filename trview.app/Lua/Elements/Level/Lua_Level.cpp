#include "Lua_Level.h"
#include "../../Lua.h"
#include "../../../Elements/ILevel.h"
#include "../Room/Lua_Room.h"
#include "../Item/Lua_Item.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            ILevel* current_level = nullptr;

            int level_index(lua_State* L)
            {
                if (!current_level)
                {
                    return 0;
                }

                const std::string key = lua_tostring(L, 2);
                if (key == "cameras_and_sinks")
                {
                    lua_newtable(L);
                    // TODO: Cameras/Sinks
                    return 1;
                }
                else if (key == "floordata")
                {
                    lua_newtable(L);
                    // TODO: Floordata
                    return 1;
                }
                else if (key == "items")
                {
                    lua_newtable(L);
                    int index = 1;
                    for (const auto& item : current_level->items())
                    {
                        lua_pushnumber(L, index);
                        create_item(L, item.lock());
                        lua_settable(L, -3);
                        ++index;
                    }
                    return 1;
                }
                else if (key == "lights")
                {
                    lua_newtable(L);
                    // TODO: Lights
                    return 1;
                }
                else if (key == "rooms")
                {
                    lua_newtable(L);
                    int index = 1;
                    for (const auto& room : current_level->rooms())
                    {
                        lua_pushnumber(L, index);
                        create_room(L, room.lock());
                        lua_settable(L, -3);
                        ++index;
                    }
                    return 1;
                }
                else if (key == "triggers")
                {
                    lua_newtable(L);
                    // TODO: Triggers
                    return 1;
                }
                else if (key == "version")
                {
                    lua_pushinteger(L, static_cast<int>(current_level->version()));
                    return 1;
                }
                else if (key == "filename")
                {
                    lua_pushstring(L, current_level->filename().c_str());
                    return 1;
                }
                return 0;
            }

            constexpr struct luaL_Reg level_lib[] =
            {
                { "__index", level_index },
                { NULL, NULL },
            };
        }

        void create_level(lua_State* L, ILevel* level)
        {
            if (!level)
            {
                lua_pushnil(L);
                return;
            }

            lua_newtable(L);
            luaL_setfuncs(L, level_lib, 0);
            lua_pushvalue(L, -1);
            lua_setmetatable(L, -2);
        }

        ILevel* level_current_level()
        {
            return current_level;
        }

        void level_set_current_level(ILevel* level)
        {
            current_level = level;
        }
    }
}
