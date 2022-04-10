#include "Lua_Item.h"
#include "../../../Elements/ILevel.h"
#include "../../Lua.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            ILevel* current_level = nullptr;
        }

        void create_item(lua_State* L, const Item& item)
        {
            lua_newtable(L);
            set_integer(L, "number", item.number());
            set_string(L, "type", to_utf8(item.type()));
        }

        void item_set_current_level(ILevel* level)
        {
            current_level = level;
        }
    }
}
