#include "Lua_Sector.h"
#include "../../../Elements/ILevel.h"
#include "../Item/Lua_Item.h"
#include "../Trigger/Lua_Trigger.h"
#include "../../Lua.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            std::unordered_map<ISector**, std::shared_ptr<ISector>> sectors;

            int sector_index(lua_State* L)
            {
                auto sector = lua::get_self<ISector>(L);

                const std::string key = lua_tostring(L, 2);
                if (key == "number")
                {
                    lua_pushinteger(L, sector->id());
                    return 1;
                }

                return 0;
            }

            int sector_newindex(lua_State* L)
            {
                auto sector = lua::get_self<ISector>(L);
                sector;
                return 0;
            }

            int sector_gc(lua_State* L)
            {
                luaL_checktype(L, 1, LUA_TUSERDATA);
                ISector** userdata = static_cast<ISector**>(lua_touserdata(L, 1));
                sectors.erase(userdata);
                return 0;
            }
        }

        int create_sector(lua_State* L, std::shared_ptr<ISector> sector)
        {
            if (!sector)
            {
                lua_pushnil(L);
                return 1;
            }

            ISector** userdata = static_cast<ISector**>(lua_newuserdata(L, sizeof(sector.get())));
            *userdata = sector.get();
            sectors[userdata] = sector;

            lua_newtable(L);
            lua_pushcfunction(L, sector_index);
            lua_setfield(L, -2, "__index");
            lua_pushcfunction(L, sector_newindex);
            lua_setfield(L, -2, "__newindex");
            lua_pushcfunction(L, sector_gc);
            lua_setfield(L, -2, "__gc");
            lua_setmetatable(L, -2);
            return 1;
        }
    }
}