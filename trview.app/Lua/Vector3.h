#pragma once

struct lua_State;

namespace trview
{
    namespace lua
    {
        int create_vector3(lua_State* L, const DirectX::SimpleMath::Vector3& value);
    }
}
