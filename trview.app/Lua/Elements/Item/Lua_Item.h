#pragma once

#include "../../../Elements/Item.h"

struct lua_State;

namespace trview
{
    namespace lua
    {
        int create_item(lua_State* L, const std::shared_ptr<IItem>& item);
        std::shared_ptr<IItem> to_item(lua_State* L, int index);
    }
}