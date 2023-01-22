#pragma once

struct lua_State;

namespace trview
{
    struct ITrigger;

    namespace lua
    {
        void create_trigger(lua_State* L, const std::shared_ptr<ITrigger>& trigger);
    }
}
