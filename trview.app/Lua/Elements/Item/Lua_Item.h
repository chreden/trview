#pragma once

#include "../../../Elements/Item.h"

struct lua_State;

namespace trview
{
    namespace lua
    {
        void create_item(lua_State* L, const Item& item);
    }
}