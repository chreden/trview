#pragma once

#include <external/lua/src/lua.hpp>
#include "../../Elements/ILevel.h"

namespace trview
{
    struct IApplication;

    namespace lua
    {
        void trview_register(lua_State* L, IApplication* application);
    }
}
