#pragma once

#include "../../Routing/IRoute.h"

struct lua_State;

namespace trview
{
    namespace lua
    {
        int create_route(lua_State* L, const std::shared_ptr<IRoute>& route);
        std::shared_ptr<IRoute> to_route(lua_State* L, int index);
        void route_register(lua_State* L, const IRoute::Source& source);
    }
}
