#pragma once

#include "../../Routing/IRoute.h"

struct lua_State;

namespace trview
{
    namespace lua
    {
        int create_route(lua_State* L, const std::shared_ptr<IRoute>& route);
    }
}
