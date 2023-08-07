#pragma once

#include "../../../Elements/ISector.h"

struct lua_State;

namespace trview
{
    namespace lua
    {
        int create_sector(lua_State* L, std::shared_ptr<ISector> sector);
        void sector_register(lua_State* L);
        std::shared_ptr<ISector> to_sector(lua_State* L, int index);
    }
}
