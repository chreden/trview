module;

#include <SimpleMath.h>
#include <external/lua/src/lua.h>

export module trview.app:LuaVector3;

import std;

namespace trview
{
    namespace lua
    {
        export int create_vector3(lua_State* L, const DirectX::SimpleMath::Vector3& value);
        export bool is_vector3(lua_State* L, int index);
        export DirectX::SimpleMath::Vector3 to_vector3(lua_State* L, int index);
        export DirectX::SimpleMath::Vector3 to_vector3(lua_State* L, int index, const std::string& name, const DirectX::SimpleMath::Vector3& default_value);
        export void vector3_register(lua_State* L);
    }
}
