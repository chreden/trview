#include "Lua_Room.h"
#include "../../../Elements/IRoom.h"
#include "../../../Elements/ILevel.h"
#include "../../Lua.h"
#include "../Sector/Lua_Sector.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            ILevel* current_level = nullptr;

            int room_index(lua_State* L)
            {
                if (!current_level)
                {
                    return 0;
                }

                const std::string key = lua_tostring(L, 2);
                if (key == "sectors")
                {
                    lua_getfield(L, 1, "number");
                    uint32_t room_number = lua_tonumber(L, -1);
                    lua_pop(L, 1);

                    const auto room = current_level->room(room_number).lock();
                    if (!room)
                    {
                        lua_pushnil(L);
                        return 1;
                    }

                    lua_newtable(L);
                    uint32_t i = 0;
                    for (const auto& sector : room->sectors())
                    {
                        create_sector(L, sector);
                        lua_rawseti(L, -2, i + 1);
                        ++i;
                    }
                    return 1;
                }
                return 0;
            }

            constexpr struct luaL_Reg room_lib[] =
            {
                { "__index", room_index },
                { NULL, NULL },
            };
        }

        void create_room(lua_State* L, const std::weak_ptr<IRoom>& room)
        {
            const auto room_ptr = room.lock();
            if (!room_ptr)
            {
                lua_pushnil(L);
                return;
            }

            lua_newtable(L);
            luaL_setfuncs(L, room_lib, 0);
            lua_pushvalue(L, -1);
            lua_setmetatable(L, -2);
            lua::set_integer(L, "number", room_ptr->number());
        }

        void room_set_current_level(ILevel* level)
        {
            current_level = level;
        }
    }
}
