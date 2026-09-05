module;

#include <SimpleMath.h>
#include <external/lua/src/lua.h>

export module trview.app:LuaVector2;

namespace trview
{
    namespace lua
    {
        export int create_vector2(lua_State* L, const DirectX::SimpleMath::Vector2& value);
        export void vector2_register(lua_State* L);
    }
}
