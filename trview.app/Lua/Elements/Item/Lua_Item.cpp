#include "Lua_Item.h"
#include "../../../Elements/ILevel.h"
#include "../Level/Lua_Level.h"
#include "../../Lua.h"
#include "../Room/Lua_Room.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            std::optional<Item> get_item(lua_State* L, ILevel* level)
            {
                lua_getfield(L, -1, "number");
                uint32_t item_number = static_cast<uint32_t>(lua_tointeger(L, -1));
                lua_pop(L, 1);
                return level->item(item_number);
            }

            int item_index(lua_State* L)
            {
                ILevel* level = level_current_level();
                if (!level)
                {
                    return 0;
                }

                auto item = get_item(L, level);
                if (!item)
                {
                    return 0;
                }

                const std::string key = lua_tostring(L, 2);
                if (key == "visible")
                {
                    lua_pushboolean(L, item.value().visible());
                    return 1;
                }
                else if (key == "room")
                {
                    create_room(L, level->room(item.value().room()).lock());
                    return 1;
                }

                return 0;
            }

            int item_newindex(lua_State* L)
            {
                ILevel* level = level_current_level();
                if (!level)
                {
                    return 0;
                }

                lua_getfield(L, 1, "number");
                uint32_t item_number = static_cast<uint32_t>(lua_tointeger(L, -1));
                lua_pop(L, 1);

                auto item = level->item(item_number);
                if (!item)
                {
                    return 0;
                }

                const std::string key = lua_tostring(L, 2);
                if (key == "visible")
                {
                    bool visibility = lua_toboolean(L, -1);
                    level->set_item_visibility(item_number, visibility);
                    return 0;
                }

                return 0;
            }

            constexpr struct luaL_Reg item_lib[] =
            {
                { "__index", item_index },
                { "__newindex", item_newindex },
                { NULL, NULL },
            };
        }

        void create_item(lua_State* L, const Item& item)
        {
            lua_newtable(L);

            lua_pushinteger(L, item.number());
            lua_setfield(L, -2, "number");

            luaL_setfuncs(L, item_lib, 0);
            lua_pushvalue(L, -1);
            lua_setmetatable(L, -2);
            /*
            // TODO: Room
            lua_pushinteger(L, item.type_id());
            lua_setfield(L, -2, "type_id");

            lua_pushstring(L, item.type().c_str());
            lua_setfield(L, -2, "type");

            lua_pushinteger(L, item.ocb());
            lua_setfield(L, -2, "ocb");

            lua_pushinteger(L, item.activation_flags());
            lua_setfield(L, -2, "activation_flags");
            */
            // TODO: Specific flags
            // TODO: Triggers
            // TODO: Position
            // TODO: Visible
        }
    }
}
