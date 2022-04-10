#include "pch.h"
#include "../../../Elements/ITrigger.h"
#include "../../../Elements/ILevel.h"
#include "../../Lua.h"
#include "../Room/Lua_Room.h"
#include "Lua_Trigger.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            ILevel* current_level;

            int trigger_index(lua_State* L)
            {
                if (!current_level)
                {
                    return 0;
                }

                const std::string key = lua_tostring(L, 2);
                return 0;
            }

            constexpr struct luaL_Reg trigger_lib[] =
            {
                { "__index", trigger_index },
                { NULL, NULL },
            };
        }

        void create_trigger(lua_State* L, const std::weak_ptr<ITrigger>& trigger) 
        {
            const auto trigger_ptr = trigger.lock();
            if (!trigger_ptr)
            {
                lua_pushnil(L);
                return;
            }

            lua_newtable(L);
            luaL_setfuncs(L, trigger_lib, 0);
            lua_pushvalue(L, -1);
            lua_setmetatable(L, -2);
            lua::set_integer(L, "flags", trigger_ptr->flags());
            lua::set_integer(L, "number", trigger_ptr->number());
            lua::set_boolean(L, "onlyOnce", trigger_ptr->only_once());
            lua::set_integer(L, "timer", trigger_ptr->timer());
            if (trigger_ptr->room() < current_level->number_of_rooms())
            {
                lua::create_room(L, current_level->rooms()[trigger_ptr->room()]);
            }
            lua::set_string(L, "type", to_utf8(trigger_type_name(trigger_ptr->type())));
        }

        void trigger_set_current_level(ILevel* level)
        {
            current_level = level;
        }
    }
}

