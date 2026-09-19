module;

#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include <SimpleMath.h>

module trview.app:LuaTrigger;

import trlevel;
import trview.lua;
import :Lua;
import :LuaColour;
import :LuaVector3;
import :LuaRoom;
import :LuaSector;

namespace trview
{
    namespace lua
    {
        namespace
        {
            int trigger_metatable = LUA_NOREF;

            void create_command(lua_State* L, const Command& command)
            {
                lua_newtable(L);
                lua_pushinteger(L, command.number());
                lua_setfield(L, -2, "number");
                lua_pushinteger(L, command.index());
                lua_setfield(L, -2, "index");
                lua_pushstring(L, command_type_name(command.type()).c_str());
                lua_setfield(L, -2, "type");
                lua_newtable(L);
                int index = 1;
                for (const auto& data : command.data())
                {
                    lua_pushnumber(L, index);
                    lua_pushnumber(L, data);
                    lua_settable(L, -3);
                    ++index;
                }
                lua_setfield(L, -2, "data");
            }

            int trigger_newindex(lua_State* L)
            {
                auto trigger = get_userdata<std::shared_ptr<ITrigger>>(L, 1);

                const std::string key = lua_tostring(L, 2);
                if (key == "colour")
                {
                    std::optional<Colour> colour;
                    if (lua_type(L, -1) != LUA_TNIL)
                    {
                        colour = to_colour(L, -1);
                    }
                    trigger->set_colour(colour);
                }
                else if (key == "visible")
                {
                    trigger->set_visible(lua_toboolean(L, -1));
                }

                return 0;
            }

            DirectX::SimpleMath::Vector3 scale_vector(const DirectX::SimpleMath::Vector3& v)
            {
                return v * trlevel::Scale;
            }

            const std::unordered_map<std::string, lua_CFunction> Functions
            {
                { "colour", prop_getter<std::shared_ptr<ITrigger>, &ITrigger::colour> },
                { "commands", prop_getter<std::shared_ptr<ITrigger>, &ITrigger::commands> },
                { "flags", prop_getter<std::shared_ptr<ITrigger>, &ITrigger::flags> },
                { "number", prop_getter<std::shared_ptr<ITrigger>, &ITrigger::number> },
                { "only_once", prop_getter<std::shared_ptr<ITrigger>, &ITrigger::only_once> },
                { "position", prop_getter_with_transform<std::shared_ptr<ITrigger>, &ITrigger::position, scale_vector> },
                { "room", prop_getter<std::shared_ptr<ITrigger>, &ITrigger::room> },
                { "sector", prop_getter<std::shared_ptr<ITrigger>, &ITrigger::sector> },
                { "timer", prop_getter<std::shared_ptr<ITrigger>, &ITrigger::timer> },
                { "type", prop_getter<std::shared_ptr<ITrigger>, &ITrigger::type> },
                { "visible", prop_getter<std::shared_ptr<ITrigger>, &ITrigger::visible> },
            };
        }

        void trigger_register(lua_State* L)
        {
            trigger_metatable = store_metatable(L,
                {
                    { "__index", default_index<Functions> },
                    { "__newindex", trigger_newindex },
                    { "__gc", default_gc<std::shared_ptr<ITrigger>> },
                });
        }

        std::shared_ptr<ITrigger> to_trigger(lua_State* L, int index)
        {
            return get_userdata<std::shared_ptr<ITrigger>>(L, index);
        }

        int to_lua(lua_State* L, const std::weak_ptr<ITrigger>& trigger)
        {
            return create_userdata(L, trigger.lock(), trigger_metatable);
        }

        int to_lua(lua_State* L, TriggerType type)
        {
            lua_pushstring(L, to_string(type).c_str());
            return 1;
        }

        int to_lua(lua_State* L, Command command)
        {
            create_command(L, command);
            return 1;
        }
    }
}
