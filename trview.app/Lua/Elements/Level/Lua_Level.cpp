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
            std::unordered_map<ILevel**, std::shared_ptr<ILevel>> levels;

            int level_index(lua_State* L)
            {
                ILevel* level = *static_cast<ILevel**>(lua_touserdata(L, 1));

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
                    for (const auto& item : level->items())
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
                    for (const auto& room : level->rooms())
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
                    lua_pushinteger(L, static_cast<int>(level->version()));
                    return 1;
                }
                else if (key == "filename")
                {
                    lua_pushstring(L, level->filename().c_str());
                    return 1;
                }
                return 0;
            }

            int level_newindex(lua_State*)
            {
                return 0;
            }

            int level_gc(lua_State* L)
            {
                ILevel** userdata = static_cast<ILevel**>(lua_touserdata(L, 1));
                levels.erase(userdata);
                return 0;
            }
        }

        void create_level(lua_State* L, const std::shared_ptr<ILevel>& level)
        {
            if (!level)
            {
                lua_pushnil(L);
                return;
            }

            ILevel** userdata = static_cast<ILevel**>(lua_newuserdata(L, sizeof(level.get())));
            *userdata = level.get();
            levels[userdata] = level;

            lua_newtable(L);
            lua_pushcfunction(L, level_index);
            lua_setfield(L, -2, "__index");
            lua_pushcfunction(L, level_newindex);
            lua_setfield(L, -2, "__newindex");
            lua_pushcfunction(L, level_gc);
            lua_setfield(L, -2, "__gc");
            lua_setmetatable(L, -2);
        }
    }
}
