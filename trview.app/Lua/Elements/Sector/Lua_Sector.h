#pragma once

struct lua_State;

namespace trview
{
    struct ILevel;
    struct ISector;

    namespace lua
    {
        void create_sector(lua_State* L, const std::weak_ptr<ISector>& room);
        void sector_set_current_level(ILevel* level);
    }
}
