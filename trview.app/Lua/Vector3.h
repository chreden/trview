#pragma once

struct lua_State;

namespace trview
{
    namespace lua
    {
        int create_vector3(lua_State* L, const DirectX::SimpleMath::Vector3& value);
        DirectX::SimpleMath::Vector3 to_vector3(lua_State* L, int index);
        DirectX::SimpleMath::Vector3 to_vector3(lua_State* L, int index, const std::string& name, const DirectX::SimpleMath::Vector3& default_value);
    }
}
