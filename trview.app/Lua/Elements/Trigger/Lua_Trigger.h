#pragma once

struct lua_State;

namespace trview
{
    struct ITrigger;

    namespace lua
    {
        int create_trigger(lua_State* L, const std::shared_ptr<ITrigger>& trigger);
        std::shared_ptr<ITrigger> to_trigger(lua_State* L, int index);
    }
}
