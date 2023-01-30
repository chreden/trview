#pragma once

struct lua_State;

namespace trview
{
    namespace lua
    {
        int create_colour(lua_State* L, const DirectX::SimpleMath::Color& value);
    }
}
