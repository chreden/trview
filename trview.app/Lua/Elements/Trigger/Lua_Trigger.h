#pragma once

struct lua_State;

namespace trview
{
    struct ILevel;
    struct ITrigger;

    namespace lua
    {
        void create_trigger(lua_State* L, const std::weak_ptr<ITrigger>& trigger);
        void trigger_set_current_level(ILevel* level);
    }
}