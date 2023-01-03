#pragma once

struct lua_State;

namespace trview
{
    struct ILevel;

    namespace lua
    {
        void create_level(lua_State* L, ILevel* level);
        void level_set_current_level(ILevel* level);
    }
}