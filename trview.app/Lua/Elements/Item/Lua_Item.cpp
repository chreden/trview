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
            std::unordered_map<IItem**, std::shared_ptr<IItem>> items;

            int item_index(lua_State* L)
            {
                IItem* item = *static_cast<IItem**>(lua_touserdata(L, 1));

                const std::string key = lua_tostring(L, 2);
                if (key == "visible")
                {
                    lua_pushboolean(L, item->visible());
                    return 1;
                }
                else if (key == "room")
                {
                    // Todo: Get the level somehow.
                    // create_room(L, level->room(item.value().room()).lock());
                    lua_pushnil(L);
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
                    bool visibility = lua_toboolean(L, -1);
                    // TODO: Do this through level, or notify level in some way?
                    item->set_visible(visibility);
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

        void create_item(lua_State* L, std::shared_ptr<IItem> item)
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
