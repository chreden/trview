#include "Lua_Trigger.h"
#include "../../Lua.h"
#include "../../../Elements/ITrigger.h"
#include "../../../Elements/ILevel.h"
#include "../../Vector3.h"
#include "../Room/Lua_Room.h"
#include "../Sector/Lua_Sector.h"
#include "../../Colour.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            void create_command(lua_State* L, const Command& command)
            {
                lua_newtable(L);
                lua_pushinteger(L, command.number());
                lua_setfield(L, -2, "number");
                lua_pushinteger(L, command.index());
                lua_setfield(L, -2, "index");
                lua_pushstring(L, command_type_name(command.type()).c_str());
                lua_setfield(L, -2, "type");
            }

            int trigger_index(lua_State* L)
            {
                auto trigger = lua::get_self<ITrigger>(L);

                const std::string key = lua_tostring(L, 2);
                if (key == "colour")
                {
                    return create_colour(L, trigger->colour());
                }
                else if (key == "commands")
                {
                    return push_list(L, trigger->commands(), create_command);
                }
                else if (key == "flags")
                {
                    lua_pushinteger(L, trigger->flags());
                    return 1;
                }
                else if (key == "number")
                {
                    lua_pushinteger(L, trigger->number());
                    return 1;
                }
                else if (key == "only_once")
                {
                    lua_pushboolean(L, trigger->only_once());
                    return 1;
                }
                else if (key == "position")
                {
                    return create_vector3(L, trigger->position() * trlevel::Scale);
                }
                else if (key == "room")
                {
                    return create_room(L, trigger->room().lock());
                }
                else if (key == "sector")
                {
                    if (auto room = trigger->room().lock())
                    {
                        const auto sectors = room->sectors();
                        if (trigger->sector_id() < sectors.size())
                        {
                            return create_sector(L, sectors[trigger->sector_id()]);
                        }
                    }
                    lua_pushnil(L);
                    return 1;
                }
                else if (key == "timer")
                {
                    lua_pushinteger(L, trigger->timer());
                    return 1;
                }
                else if (key == "type")
                {
                    lua_pushstring(L, trigger_type_name(trigger->type()).c_str());
                    return 1;
                }
                else if (key == "visible")
                {
                    lua_pushboolean(L, trigger->visible());
                    return 1;
                }

                return 0;
            }

            int trigger_newindex(lua_State* L)
            {
                auto trigger = lua::get_self<ITrigger>(L);

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
        }

        int create_trigger(lua_State* L, const std::shared_ptr<ITrigger>& trigger)
        {
            return create(L, trigger, trigger_index, trigger_newindex);
        }

        std::shared_ptr<ITrigger> to_trigger(lua_State* L, int index)
        {
            return get_self<ITrigger>(L, index);
        }
    }
}
