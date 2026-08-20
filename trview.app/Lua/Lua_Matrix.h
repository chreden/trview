#pragma once

struct lua_State;

namespace trview
{
    namespace lua
    {
        int create_matrix(lua_State* L, const DirectX::SimpleMath::Matrix& value);
        bool is_matrix(lua_State* L, int index);
        void matrix_register(lua_State* L);
    }
}
