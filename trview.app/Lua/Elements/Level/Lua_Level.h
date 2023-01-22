#pragma once

struct lua_State;

namespace trview
{
    struct ILevel;

    namespace lua
    {
        void create_level(lua_State* L, const std::shared_ptr<ILevel>& level);
    }
}