#pragma once

#include "../../../Elements/ISector.h"

struct lua_State;

namespace trview
{
    namespace lua
    {
        int create_sector(lua_State* L, std::shared_ptr<ISector> sector);
    }
}
