#include "Lua_Trigger.h"
#include "../../Lua.h"
#include "../../../Elements/ITrigger.h"
#include "../../../Elements/ILevel.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            std::unordered_map<ITrigger**, std::shared_ptr<ITrigger>> triggers;

            int trigger_index(lua_State* L)
            {
                ITrigger* trigger = *static_cast<ITrigger**>(lua_touserdata(L, 1));

                const std::string key = lua_tostring(L, 2);
                if (key == "number")
                {
                    lua_pushinteger(L, trigger->number());
                    return 1;
                }

                return 0;
            }

            int trigger_newindex(lua_State* L)
            {
                ITrigger* trigger = *static_cast<ITrigger**>(lua_touserdata(L, 1));

                const std::string key = lua_tostring(L, 2);
                if (key == "visible")
                {
                    if (auto level = trigger->level().lock())
                    {
                        level->set_trigger_visibility(trigger->number(), lua_toboolean(L, -1));
                    }
                }

                return 0;
            }

            int trigger_gc(lua_State* L)
            {
                ITrigger** userdata = static_cast<ITrigger**>(lua_touserdata(L, 1));
                triggers.erase(userdata);
                return 0;
            }
        }

        void create_trigger(lua_State* L, const std::shared_ptr<ITrigger>& trigger)
        {
            if (!trigger)
            {
                lua_pushnil(L);
                return;
            }

            ITrigger** userdata = static_cast<ITrigger**>(lua_newuserdata(L, sizeof(trigger.get())));
            *userdata = trigger.get();
            triggers[userdata] = trigger;

            lua_newtable(L);
            lua_pushcfunction(L, trigger_index);
            lua_setfield(L, -2, "__index");
            lua_pushcfunction(L, trigger_newindex);
            lua_setfield(L, -2, "__newindex");
            lua_pushcfunction(L, trigger_gc);
            lua_setfield(L, -2, "__gc");
            lua_setmetatable(L, -2);
        }
    }
}
