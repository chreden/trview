#include "Lua_Item.h"
#include "../../Lua.h"
#include "../../../Elements/ILevel.h"
#include "../Level/Lua_Level.h"
#include "../Room/Lua_Room.h"
#include "../Trigger/Lua_Trigger.h"
#include "../../Vector3.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            std::unordered_map<IItem**, std::shared_ptr<IItem>> items;

            int item_index(lua_State* L)
            {
                IItem* item = *static_cast<IItem**>(lua_touserdata(L, 1));

                const std::string key = lua_tostring(L, 2);
                if (key == "activation_flags")
                {
                    lua_pushinteger(L, item->activation_flags());
                    return 1;
                }
                else if (key == "number")
                {
                    lua_pushinteger(L, item->number());
                    return 1;
                }
                else if (key == "ocb")
                {
                    lua_pushinteger(L, item->ocb());
                    return 1;
                }
                else if (key == "position")
                {
                    return create_vector3(L, item->position() * trlevel::Scale);
                }
                else if (key == "room")
                {
                    if (auto level = item->level().lock())
                    {
                        return create_room(L, level->room(item->room()).lock());
                    }
                    lua_pushnil(L);
                    return 1;
                }
                else if (key == "triggered_by")
                {
                    return push_list(L, item->triggers(), { create_trigger });
                }
                else if (key == "type")
                {
                    lua_pushstring(L, item->type().c_str());
                    return 1;
                }
                else if (key == "type_id")
                {
                    lua_pushinteger(L, item->type_id());
                    return 1;
                }
                else if (key == "visible")
                {
                    lua_pushboolean(L, item->visible());
                    return 1;
                }

                return 0;
            }

            int item_newindex(lua_State* L)
            {
                IItem* item = *static_cast<IItem**>(lua_touserdata(L, 1));

                const std::string key = lua_tostring(L, 2);
                if (key == "visible")
                {
                    if (auto level = item->level().lock())
                    {
                        level->set_item_visibility(item->number(), lua_toboolean(L, -1));
                    }
                    return 0;
                }

                return 0;
            }

            int item_gc(lua_State* L)
            {
                IItem** userdata = static_cast<IItem**>(lua_touserdata(L, 1));
                items.erase(userdata);
                return 0;
            }
        }

        void create_item(lua_State* L, const std::shared_ptr<IItem>& item)
        {
            if (!item)
            {
                lua_pushnil(L);
                return;
            }

            IItem** userdata = static_cast<IItem**>(lua_newuserdata(L, sizeof(item.get())));
            *userdata = item.get();
            items[userdata] = item;

            lua_newtable(L);
            lua_pushcfunction(L, item_index);
            lua_setfield(L, -2, "__index");
            lua_pushcfunction(L, item_newindex);
            lua_setfield(L, -2, "__newindex");
            lua_pushcfunction(L, item_gc);
            lua_setfield(L, -2, "__gc");
            lua_setmetatable(L, -2);

            // TODO: Specific flags
            // TODO: Position
        }
    }
}
