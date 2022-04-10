#include "Lua_Level.h"
#include "../../../Elements/ILevel.h"
#include "../../Lua.h"
#include "../Room/Lua_Room.h"
#include "../Item/Lua_Item.h"
#include "../Trigger/Lua_Trigger.h"

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
                if (key == "items")
                {
                    lua_newtable(L);
                    uint32_t i = 0;
                    for (const auto& item : current_level->items())
                    {
                        create_item(L, item);
                        lua_rawseti(L, -2, i + 1);
                        ++i;
                    }
                    return 1;
                }
                else if (key == "rooms")
                {
                    lua_newtable(L);
                    uint32_t i = 0;
                    for (const auto& room : current_level->rooms())
                    {
                        create_room(L, room);
                        lua_rawseti(L, -2, i + 1);
                        ++i;
                    }
                    return 1;
                }
                else if (key == "triggers")
                {
                    lua_newtable(L);
                    uint32_t i = 0;
                    for (const auto& trigger : current_level->triggers())
                    {
                        create_trigger(L, trigger);
                        lua_rawseti(L, -2, i + 1);
                        ++i;
                    }
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

        void level_set_current_level(ILevel* level)
        {
            current_level = level;
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
    }
}