#pragma once

struct lua_State;

namespace trview
{
    struct ILevel;

    namespace lua
    {
        int create_level(lua_State* L, const std::shared_ptr<ILevel>& level);
        std::shared_ptr<ILevel> to_level(lua_State* L, int index);
    }
}