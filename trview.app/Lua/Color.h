#pragma once

struct lua_State;

namespace trview
{
    namespace lua
    {
        int create_color(lua_State* L, const DirectX::SimpleMath::Color& value);
    }
}
