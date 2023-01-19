#pragma once

#include <external/lua/src/lua.hpp>
#include "../../Elements/ILevel.h"

namespace trview
{
    namespace lua
    {
        void trview_register(lua_State* L);
        void set_current_level(ILevel* level);
    }
}
