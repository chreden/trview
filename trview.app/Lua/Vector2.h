#pragma once

struct lua_State;

namespace trview
{
    namespace lua
    {
        int create_vector2(lua_State* L, const DirectX::SimpleMath::Vector2& value);
        void vector2_register(lua_State* L);
    }
}
